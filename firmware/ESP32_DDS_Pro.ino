/**
 * ESP32 Advanced DDS Signal Generator
 * Version: 1.0.0
 * 
 * Features:
 * - Precision frequency generation (0.01Hz - 5MHz)
 * - Sine/Triangle/Square waveforms
 * - OLED display with real-time parameters
 * - Web-based remote control interface
 * - Rotary encoder for local control
 * - Signal processing and calibration
 */

#include <SPI.h>
#include <WiFi.h>
#include <ESP32Encoder.h>
#include "src/ad9833_controller.h"
#include "src/web_interface.h"
#include "src/oled_ui.h"
#include "src/signal_processor.h"

// ================= Hardware Configuration =================
#define AD9833_FSYNC   5   // FSYNC pin for AD9833
#define ENC_A          16  // Rotary encoder A phase
#define ENC_B          17  // Rotary encoder B phase
#define ENC_SW         4   // Encoder switch button
#define OLED_SDA       21  // OLED SDA pin
#define OLED_SCL       22  // OLED SCL pin

// ================= Global Variables & Constants =================
const unsigned long DISPLAY_REFRESH = 250;  // ms between display updates
const float FREQ_STEP_SMALL = 0.1;          // Fine adjustment step
const float FREQ_STEP_LARGE = 100.0;         // Coarse adjustment step

enum Waveform { SINE, TRIANGLE, SQUARE };
Waveform currentWaveform = SINE;

volatile float currentFrequency = 1000.0;    // Default 1kHz
volatile bool frequencyChanged = false;
volatile bool waveformChanged = false;
volatile bool encoderPressed = false;

unsigned long lastDisplayUpdate = 0;
String apIP = "";

// ================= Object Instances =================
AD9833Controller dds(AD9833_FSYNC);
OLED_UI display(OLED_SDA, OLED_SCL);
WebInterface webInterface(dds);
SignalProcessor signalProcessor;
ESP32Encoder encoder;

// ================= Interrupt Service Routines =================
void IRAM_ATTR handleEncoder() {
  static int lastCount = 0;
  int newCount = encoder.getCount();
  
  if (newCount > lastCount) {
    currentFrequency += (currentFrequency < 1000) ? FREQ_STEP_SMALL : FREQ_STEP_LARGE;
  } else if (newCount < lastCount) {
    currentFrequency -= (currentFrequency <= 1000) ? FREQ_STEP_SMALL : FREQ_STEP_LARGE;
  }
  
  currentFrequency = constrain(currentFrequency, 0.01, 5000000);
  lastCount = newCount;
  frequencyChanged = true;
}

void IRAM_ATTR handleEncoderSwitch() {
  encoderPressed = true;
}

// ================= Waveform Management =================
String getWaveformString() {
  switch(currentWaveform) {
    case SINE: return "Sine";
    case TRIANGLE: return "Triangle";
    case SQUARE: return "Square";
    default: return "Unknown";
  }
}

void changeWaveform() {
  currentWaveform = static_cast<Waveform>((currentWaveform + 1) % 3);
  waveformChanged = true;
}

// ================= DDS Update Handler =================
void updateDDSOutput() {
  uint16_t waveformMode;
  
  switch(currentWaveform) {
    case SINE: waveformMode = AD9833_SINE; break;
    case TRIANGLE: waveformMode = AD9833_TRIANGLE; break;
    case SQUARE: waveformMode = AD9833_SQUARE; break;
  }
  
  dds.setFrequency(currentFrequency, waveformMode);
}

// ================= Initialization Functions =================
void initEncoder() {
  encoder.attachHalfQuad(ENC_A, ENC_B);
  encoder.setCount(0);
  attachInterrupt(digitalPinToInterrupt(ENC_A), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), handleEncoder, CHANGE);
  pinMode(ENC_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_SW), handleEncoderSwitch, FALLING);
}

void initSystem() {
  Serial.begin(115200);
  Serial.println("\n\nInitializing DDS Signal Generator...");
  
  // Initialize DDS module
  dds.begin();
  dds.setFrequency(currentFrequency, AD9833_SINE);
  
  // Initialize OLED display
  if (!display.begin()) {
    Serial.println("OLED initialization failed! System halted");
    while(true); // Critical failure - halt system
  }
  display.showSplashScreen();
  
  // Initialize signal processing
  signalProcessor.setLowPassFilter(1000000); // 1MHz cutoff
  signalProcessor.calibrateDCOffset();
  
  // Initialize rotary encoder
  initEncoder();
  
  // Start WiFi and web interface
  webInterface.begin();
  apIP = WiFi.softAPIP().toString();
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(apIP);
  
  delay(1000); // Allow hardware to stabilize
}

// ================= Main Functions =================
void setup() {
  initSystem();
  Serial.println("System initialization complete");
}

void loop() {
  // Handle DDS updates
  if (frequencyChanged || waveformChanged) {
    updateDDSOutput();
    frequencyChanged = false;
    waveformChanged = false;
    Serial.print("DDS Updated: ");
    Serial.print(currentFrequency);
    Serial.print(" Hz, Waveform: ");
    Serial.println(getWaveformString());
  }
  
  // Handle encoder button press
  if (encoderPressed) {
    changeWaveform();
    encoderPressed = false;
    Serial.println("Waveform changed");
  }
  
  // Update display periodically
  if (millis() - lastDisplayUpdate > DISPLAY_REFRESH) {
    display.update(currentFrequency, getWaveformString(), apIP);
    lastDisplayUpdate = millis();
  }
  
  // Add small delay to prevent watchdog triggers
  delay(10);
}
