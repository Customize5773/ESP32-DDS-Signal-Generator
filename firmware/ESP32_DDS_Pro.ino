/**
 * ESP32 Advanced DDS Signal Generator
 * Version: 1.0.0
 * Author: Ras Rizkov (aka Rasya Pratama)
 * 
 * Features:
 * - AD9833 DDS Control (0.01Hz-5MHz)
 * - Waveform Selection (Sine/Triangle/Square)
 * - OLED Display Interface
 * - Rotary Encoder Control
 * - Web Server for Remote Control
 * - SPI Debugging Support
 */

#include <SPI.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Encoder.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "ad9833.h"

// =============== Hardware Configuration ===============
#define SPI_SCK   18
#define SPI_MOSI  23
#define AD9833_FSYNC 5

#define ENC_A     16
#define ENC_B     17
#define ENC_SW    4

#define OLED_SDA  21
#define OLED_SCL  22
#define OLED_RST  -1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// =============== Constants & Globals ===============
const char* ssid = "DDS_Generator";
const char* password = "generate123";

enum Waveform { SINE, TRIANGLE, SQUARE };
Waveform currentWave = SINE;

float frequency = 1000.0;  // Default 1kHz
bool updateDDS = true;
bool encoderPressed = false;
unsigned long lastUpdate = 0;
const long displayRefresh = 200;  // ms

// =============== Object Instances ===============
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);
ESP32Encoder encoder;
AsyncWebServer server(80);
AD9833 dds(AD9833_FSYNC);

// =============== Rotary Encoder ISR ===============
void IRAM_ATTR handleEncoder() {
  static int lastCount = 0;
  int newCount = encoder.getCount();
  
  if (newCount > lastCount) {
    frequency += (frequency < 1000) ? 0.1 : 100;
  } else if (newCount < lastCount) {
    frequency -= (frequency <= 1000) ? 0.1 : 100;
  }
  
  frequency = constrain(frequency, 0.01, 5000000);
  lastCount = newCount;
  updateDDS = true;
}

void IRAM_ATTR handleEncoderSwitch() {
  encoderPressed = true;
}

// =============== DDS Control Functions ===============
void updateOutput() {
  switch(currentWave) {
    case SINE:
      dds.setFrequency(frequency, AD9833_SINE);
      break;
    case TRIANGLE:
      dds.setFrequency(frequency, AD9833_TRIANGLE);
      break;
    case SQUARE:
      dds.setFrequency(frequency, AD9833_SQUARE);
      break;
  }
}

void cycleWaveform() {
  currentWave = static_cast<Waveform>((currentWave + 1) % 3);
  updateDDS = true;
}

// =============== OLED Display Functions ===============
void initDisplay() {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED allocation failed!");
    while(true); // Halt on critical failure
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.display();
}

void updateDisplay() {
  display.clearDisplay();
  
  // Header
  display.setCursor(0,0);
  display.print("DDS Signal Generator");
  
  // Frequency
  display.setCursor(0,20);
  display.setTextSize(2);
  display.print(frequency);
  display.setTextSize(1);
  display.println(" Hz");
  
  // Waveform
  display.setCursor(0,45);
  display.print("Wave: ");
  switch(currentWave) {
    case SINE: display.println("Sine"); break;
    case TRIANGLE: display.println("Triangle"); break;
    case SQUARE: display.println("Square"); break;
  }
  
  // Footer
  display.setCursor(0,56);
  display.print("IP: ");
  display.println(WiFi.softAPIP());
  
  display.display();
}

// =============== Web Server Handlers ===============
void initWebServer() {
  // Handle frequency updates
  server.on("/set", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("freq")) {
      frequency = request->getParam("freq")->value().toFloat();
      frequency = constrain(frequency, 0.01, 5000000);
      updateDDS = true;
    }
    request->send(200, "text/plain", "OK");
  });
  
  // Handle waveform changes
  server.on("/wave", HTTP_GET, [](AsyncWebServerRequest *request) {
    if (request->hasParam("type")) {
      String wave = request->getParam("type")->value();
      if (wave == "sine") currentWave = SINE;
      else if (wave == "tri") currentWave = TRIANGLE;
      else if (wave == "sqr") currentWave = SQUARE;
      updateDDS = true;
    }
    request->send(200, "text/plain", "OK");
  });
  
  // Serve web interface
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", WEB_INTERFACE);
  });
  
  server.begin();
}

// =============== Main Setup ===============
void setup() {
  Serial.begin(115200);
  while(!Serial); // Wait for serial port in debug mode
  
  // Initialize SPI and DDS
  SPI.begin(SPI_SCK, -1, SPI_MOSI, AD9833_FSYNC);
  dds.begin();
  dds.reset();
  updateOutput();
  
  // Initialize display
  initDisplay();
  
  // Initialize encoder
  encoder.attachHalfQuad(ENC_A, ENC_B);
  encoder.setCount(0);
  attachInterrupt(digitalPinToInterrupt(ENC_A), handleEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(ENC_B), handleEncoder, CHANGE);
  pinMode(ENC_SW, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_SW), handleEncoderSwitch, FALLING);
  
  // Start WiFi AP
  WiFi.softAP(ssid, password);
  
  // Initialize web server
  initWebServer();
  
  Serial.println("System initialized");
}

// =============== Main Loop ===============
void loop() {
  // Handle DDS updates
  if (updateDDS) {
    updateOutput();
    updateDDS = false;
  }
  
  // Handle encoder button press
  if (encoderPressed) {
    cycleWaveform();
    encoderPressed = false;
  }
  
  // Update display periodically
  if (millis() - lastUpdate > displayRefresh) {
    updateDisplay();
    lastUpdate = millis();
  }
  
  // Add small delay to prevent watchdog triggers
  delay(10);
}

// =============== Web Interface HTML ===============
const char WEB_INTERFACE[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>DDS Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin-top: 20px; }
    .slider { width: 80%; margin: 20px; }
    .wave-btn { padding: 15px; margin: 10px; font-size: 1.2em; }
  </style>
</head>
<body>
  <h1>DDS Signal Generator</h1>
  
  <div>
    <h2 id="freq-display">1000 Hz</h2>
    <input type="range" min="1" max="5000000" value="1000" 
           class="slider" id="freq-slider">
  </div>
  
  <div>
    <button class="wave-btn" onclick="setWave('sine')">Sine</button>
    <button class="wave-btn" onclick="setWave('tri')">Triangle</button>
    <button class="wave-btn" onclick="setWave('sqr')">Square</button>
  </div>
  
  <script>
    const freqSlider = document.getElementById('freq-slider');
    const freqDisplay = document.getElementById('freq-display');
    
    freqSlider.addEventListener('input', function() {
      const freq = this.value;
      freqDisplay.textContent = freq + ' Hz';
      fetch(`/set?freq=${freq}`);
    });
    
    function setWave(waveType) {
      fetch(`/wave?type=${waveType}`);
    }
  </script>
</body>
</html>
)rawliteral";
