#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include <AccelStepper.h>

// Define pin connections
#define IN1 21
#define IN2 19
#define IN3 18
#define IN4 5

// Access Point credentials
const char* ssid = "ESP32-WebServer";     // Name of the WiFi network
const char* password = "12345678";         // Password (min 8 characters, or "" for open)

// Create web server on port 80
WebServer server(80);

// GPIO pins
const int ledPin = 2;  // Built-in LED on most ESP32 boards
bool ledState = false;

// Create stepper object
// AccelStepper::HALF4WIRE = 8 (half-step, 4-wire)
AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

void setup() {
  Serial.begin(115200);
  Serial.println();
  
  // Configure LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  // Initialize LittleFS
  if(!LittleFS.begin(true)){
    Serial.println("An Error has occurred while mounting LittleFS");
    return;
  }
  Serial.println("LittleFS mounted successfully");
  
  // List files in LittleFS (for debugging)
  listFiles();
  
  // Configure Access Point with custom IP
  Serial.print("Setting up Access Point ... ");
  
  // Set custom IP address - something easy to remember!
  IPAddress local_IP(8, 8, 8, 8);        // Easy: 8.8.8.8
  IPAddress gateway(8, 8, 8, 8);         // Same as IP
  IPAddress subnet(255, 255, 255, 0);    // Standard subnet
  
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
  
  // Handle 404 for any other requests
  server.onNotFound([]() {
    server.send(404, "text/plain", "Not Found");
  });
  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
  Serial.println("Connect to WiFi network: " + String(ssid));
  Serial.println("Open browser and go to: http://" + IP.toString());

  // Set maximum speed (steps per second)
  stepper.setMaxSpeed(1000);
  
  // Set acceleration (steps per second per second)
  stepper.setAcceleration(500);
  
  // Set initial speed (steps per second)
  stepper.setSpeed(1000);
  
  Serial.println("28BYJ-48 Stepper Motor - AccelStepper Library");
  Serial.println("Motor will start spinning continuously...");
}

void loop() {
  server.handleClient();
  stepper.runSpeed();
}

// List all files in LittleFS (for debugging)
void listFiles() {
  File root = LittleFS.open("/");
  File file = root.openNextFile();
  
  Serial.println("Files in LittleFS:");
  while(file) {
    Serial.print("  ");
    Serial.print(file.name());
    Serial.print(" - ");
    Serial.print(file.size());
    Serial.println(" bytes");
    file = root.openNextFile();
  }
}

// Handle root page - serve index.html from LittleFS
void handleRoot() {
  File file = LittleFS.open("/index.html", "r");
  if (!file) {
    Serial.println("Failed to open index.html");
    server.send(404, "text/plain", "File Not Found");
    return;
  }
  
  // Send file to client
  server.streamFile(file, "text/html");
  file.close();
}

// Handle LED toggle
void handleToggle() {
  ledState = !ledState;
  digitalWrite(ledPin, ledState);
  server.send(200, "text/plain", ledState ? "ON" : "OFF");
  Serial.println("LED toggled to: " + String(ledState ? "ON" : "OFF"));

  stepper.setSpeed(ledState ? 1000 : 0);
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
