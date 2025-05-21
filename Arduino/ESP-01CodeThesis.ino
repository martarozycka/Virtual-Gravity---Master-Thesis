#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

// First in order to programme the firmware of the ESP-01 or (ESP8266 WiFi module). 1. First make sure you connect everything correctly: 
// Vcc , RST, EN -> 3.3V  || GPIO0 -> GND ||  RX -> RX (1kohm and 2.2 kohm voltage divider) || TX -> TX of Arduino Uno || GND -> GND 
// 2. Make sure you are connected to the right board being the Generic ESP8266 board 
// 3. The port is the right COM port connected to the Arduino UNO 
// Press and keep pressing the reset button before and during uploading the code. Make sure everything is connected properly . To make sure it works fine, reset the Arduino Uno board before uploading 
// the code and then keep the reset button pushed till it uploads the code.  
// Use a 2.4GHz Wi-Fi network

// To let it run: 
// 1. Disconnect Arduino. First make sure you connect everything correctly. Now to use the serial monitor , we need to use the software digital pins, pin 2 and 3 respectively: 
// Vcc , EN -> 3.3V || GPIO0, RST -> Disconnect || RX -> pin 3 (1kohm and 2.2 kohm voltage divider) || TX -> pin 2 || GND -> GND 
// 2. Open a new script 
// 3. Connect to Arduino Uno board. 
// 4. Make sure the port is correct. 
// 5. Open Serial monitor to check that the code is running on the ESP-01 or ESP8266 WiFi module  

const char* ssid = "NetworkName";
const char* password = "NetworkPassword";

// Make it a server that listens on port 80 (default for HTTP)
ESP8266WebServer server(80);

// The handleCommand function is called when a client makes a GET request to /command.
void handleCommand() {
  // Check if the 'msg' query parameter exists
  if (server.hasArg("msg")) {
    // Retrieve the value of 'msg'
    String command = server.arg("msg");
    
    // Send the command over Serial to the Arduino Uno
    Serial.println(command);
    
    // Respond to the HTTP client with confirmation
    server.send(200, "text/plain", "Received: " + command);
  } else {
    // Send an error response if the 'msg' parameter is missing
    server.send(400, "text/plain", "Error: Missing 'msg' parameter");
  }
}

void setup() {
  // Start serial communication (must match Arduino’s baud rate)
  Serial.begin(9600);
  delay(10);

  // Set the ESP8266 Wi-Fi mode to Station 
  WiFi.mode(WIFI_STA);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Begin Wi-Fi connection
  WiFi.begin(ssid, password);

  // Wait here until the ESP8266 is connected to Wi-Fi
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Once connected, print out the IP address (for debugging)
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("ESP8266 IP Address: ");
  Serial.println(WiFi.localIP());

  // Set up the web server route: when a client accesses "/command", call handleCommand()
  server.on("/command", handleCommand);

  // Start the HTTP server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Continuously process incoming HTTP client requests from Unity
  server.handleClient();
}

