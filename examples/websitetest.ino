#include <WiFi.h>
#include <WebServer.h>

// Access Point credentials
const char* ssid = "ESP32-WebServer";     // Name of the WiFi network
const char* password = "12345678";         // Password (min 8 characters, or "" for open)

// Create web server on port 80
WebServer server(80);

// GPIO pins
const int ledPin = 2;  // Built-in LED on most ESP32 boards
bool ledState = false;

// HTML webpage
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 Control Panel</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin: 0;
      padding: 20px;
      background-color: #f0f0f0;
    }
    .container {
      max-width: 600px;
      margin: 0 auto;
      background-color: white;
      padding: 20px;
      border-radius: 10px;
      box-shadow: 0 2px 10px rgba(0,0,0,0.1);
    }
    h1 {
      color: #333;
      margin-bottom: 30px;
    }
    .button {
      display: inline-block;
      padding: 15px 30px;
      font-size: 18px;
      cursor: pointer;
      text-align: center;
      text-decoration: none;
      outline: none;
      color: #fff;
      background-color: #4CAF50;
      border: none;
      border-radius: 5px;
      box-shadow: 0 5px #999;
      margin: 10px;
      transition: all 0.3s;
    }
    .button:hover {
      background-color: #3e8e41;
    }
    .button:active {
      background-color: #3e8e41;
      box-shadow: 0 2px #666;
      transform: translateY(3px);
    }
    .button.off {
      background-color: #f44336;
    }
    .button.off:hover {
      background-color: #da190b;
    }
    .status {
      font-size: 24px;
      margin: 20px 0;
      padding: 10px;
      background-color: #e7f3ff;
      border-radius: 5px;
    }
    .info {
      margin-top: 30px;
      padding: 15px;
      background-color: #fff3cd;
      border-radius: 5px;
      text-align: left;
    }
    .sensor-data {
      display: grid;
      grid-template-columns: repeat(auto-fit, minmax(150px, 1fr));
      gap: 15px;
      margin: 20px 0;
    }
    .sensor-box {
      padding: 15px;
      background-color: #e3f2fd;
      border-radius: 5px;
    }
    .sensor-value {
      font-size: 28px;
      font-weight: bold;
      color: #1976d2;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>ESP32 Access Point</h1>
    
    <div class="status">
      LED Status: <span id="ledState">OFF</span>
    </div>
    
    <button class="button" onclick="toggleLED()">Toggle LED</button>
    <button class="button off" onclick="turnOff()">Turn OFF</button>
    
    <div class="sensor-data">
      <div class="sensor-box">
        <div>Temperature</div>
        <div class="sensor-value"><span id="temp">--</span>°C</div>
      </div>
      <div class="sensor-box">
        <div>Random Value</div>
        <div class="sensor-value"><span id="random">--</span></div>
      </div>
    </div>
    
    <div class="info">
      <h3>Connection Info:</h3>
      <p><strong>SSID:</strong> ESP32-WebServer</p>
      <p><strong>IP Address:</strong> 8.8.8.8</p>
      <p><strong>Connected Clients:</strong> <span id="clients">0</span></p>
    </div>
  </div>

  <script>
    // Update sensor data every 2 seconds
    setInterval(function() {
      getSensorData();
      getClients();
    }, 2000);
    
    function toggleLED() {
      fetch('/toggle')
        .then(response => response.text())
        .then(data => {
          document.getElementById('ledState').innerHTML = data;
          updateButtonColor(data);
        });
    }
    
    function turnOff() {
      fetch('/off')
        .then(response => response.text())
        .then(data => {
          document.getElementById('ledState').innerHTML = data;
          updateButtonColor(data);
        });
    }
    
    function getSensorData() {
      fetch('/sensors')
        .then(response => response.json())
        .then(data => {
          document.getElementById('temp').innerHTML = data.temperature;
          document.getElementById('random').innerHTML = data.random;
        });
    }
    
    function getClients() {
      fetch('/clients')
        .then(response => response.text())
        .then(data => {
          document.getElementById('clients').innerHTML = data;
        });
    }
    
    function updateButtonColor(state) {
      // Optional: Update button colors based on state
    }
    
    // Initial load
    getSensorData();
    getClients();
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  // Configure LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Configure Access Point with custom IP
  Serial.print("Setting up Access Point ... ");
  
  // Set custom IP address - something easy to remember!
  IPAddress local_IP(8, 8, 8, 8);        // Easy: 8.8.8.8
  IPAddress gateway(8, 8, 8, 8);         // Same as IP
  IPAddress subnet(255, 255, 255, 0);    // Standard subnet
  
  // Alternative easy IPs:
  // IPAddress local_IP(1, 2, 3, 4);     // Sequential: 1.2.3.4
  // IPAddress local_IP(10, 10, 10, 10); // Repeating: 10.10.10.10
  // IPAddress local_IP(123, 45, 67, 89); // Pattern: 123.45.67.89
  
  // Configure the soft AP with custom IP
  WiFi.softAPConfig(local_IP, gateway, subnet);
  WiFi.softAP(ssid, password);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/off", handleOff);
  server.on("/sensors", handleSensors);
  server.on("/clients", handleClients);
  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Connect to WiFi network: " + String(ssid));
  Serial.println("Open browser and go to: http://" + IP.toString());
}

void loop() {
  server.handleClient();
}

// Handle root page
void handleRoot() {
  server.send(200, "text/html", index_html);
}

// Handle LED toggle
void handleToggle() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
  server.send(200, "text/plain", ledState ? "ON" : "OFF");
  Serial.println("LED toggled to: " + String(ledState ? "ON" : "OFF"));
}

// Handle LED off
void handleOff() {
  ledState = false;
  digitalWrite(ledPin, LOW);
  server.send(200, "text/plain", "OFF");
  Serial.println("LED turned OFF");
}

// Handle sensor data (simulated)
void handleSensors() {
  // Simulate sensor readings
  float temperature = 20.0 + (random(100) / 10.0);  // 20.0 to 30.0
  int randomValue = random(0, 100);
  
  String json = "{";
  json += "\"temperature\":" + String(temperature, 1) + ",";
  json += "\"random\":" + String(randomValue);
  json += "}";
  
  server.send(200, "application/json", json);
}

// Handle connected clients count
void handleClients() {
  int numClients = WiFi.softAPgetStationNum();
  server.send(200, "text/plain", String(numClients));
}
