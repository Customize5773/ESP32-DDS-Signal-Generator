#include "web_interface.h"

WebInterface::WebInterface(AD9833Controller& dds) 
    : server_(80), dds_(dds) {}

void WebInterface::begin() {
    // Start WiFi AP
    WiFi.softAP("DDS_Generator", "generate123");
    
    // Setup routes
    server_.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, "text/html", WEB_INTERFACE_HTML);
    });
    
    server_.on("/frequency", HTTP_GET, [this](AsyncWebServerRequest *request) {
        if (request->hasParam("value")) {
            float freq = request->getParam("value")->value().toFloat();
            dds_.setFrequency(freq);
            request->send(200, "text/plain", "OK");
        } else {
            request->send(400, "text/plain", "Missing frequency parameter");
        }
    });
    
    server_.on("/waveform", HTTP_GET, [this](AsyncWebServerRequest *request) {
        if (request->hasParam("type")) {
            String wave = request->getParam("type")->value();
            if (wave == "sine") dds_.setFrequency(0, AD9833_SINE);
            else if (wave == "triangle") dds_.setFrequency(0, AD9833_TRIANGLE);
            else if (wave == "square") dds_.setFrequency(0, AD9833_SQUARE);
            request->send(200, "text/plain", "OK");
        } else {
            request->send(400, "text/plain", "Missing waveform type");
        }
    });
    
    server_.on("/calibrate", HTTP_GET, [](AsyncWebServerRequest *request) {
        // Placeholder for calibration endpoint
        request->send(200, "text/plain", "Calibration not implemented yet");
    });
    
    server_.begin();
}

const char WebInterface::WEB_INTERFACE_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>DDS Control</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body { font-family: Arial; text-align: center; margin-top: 20px; }
    .slider { width: 80%; margin: 20px; }
    .wave-btn { padding: 15px; margin: 10px; font-size: 1.2em; }
    .status { margin-top: 30px; padding: 10px; background: #f0f0f0; }
  </style>
</head>
<body>
  <h1>ESP32 DDS Generator</h1>
  
  <div>
    <h2 id="freq-display">1000 Hz</h2>
    <input type="range" min="1" max="5000000" value="1000" 
           class="slider" id="freq-slider">
  </div>
  
  <div>
    <button class="wave-btn" onclick="setWave('sine')">Sine</button>
    <button class="wave-btn" onclick="setWave('triangle')">Triangle</button>
    <button class="wave-btn" onclick="setWave('square')">Square</button>
  </div>
  
  <div class="status">
    <p>Device Status: <span id="device-status">Operational</span></p>
    <p>IP: <span id="device-ip">192.168.4.1</span></p>
  </div>
  
  <script>
    const freqSlider = document.getElementById('freq-slider');
    const freqDisplay = document.getElementById('freq-display');
    const deviceStatus = document.getElementById('device-status');
    
    // Set initial IP display
    document.getElementById('device-ip').textContent = window.location.hostname;
    
    freqSlider.addEventListener('input', function() {
      const freq = this.value;
      freqDisplay.textContent = freq + ' Hz';
      
      // Send frequency update
      fetch(`/frequency?value=${freq}`)
        .then(response => {
          if(!response.ok) deviceStatus.textContent = 'Update failed';
          else deviceStatus.textContent = 'Frequency updated';
        })
        .catch(() => deviceStatus.textContent = 'Network error');
    });
    
    function setWave(waveType) {
      deviceStatus.textContent = 'Updating waveform...';
      fetch(`/waveform?type=${waveType}`)
        .then(response => {
          if(response.ok) deviceStatus.textContent = 'Waveform updated';
          else deviceStatus.textContent = 'Update failed';
        })
        .catch(() => deviceStatus.textContent = 'Network error');
    }
    
    // Update status periodically
    setInterval(() => {
      deviceStatus.textContent = 'Operational';
    }, 10000);
  </script>
</body>
</html>
)rawliteral";
