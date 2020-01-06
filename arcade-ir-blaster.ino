/*
  To upload through terminal you can use: curl -F "image=@firmware.bin" esp8266-webupdate.local/update
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ESP8266HTTPUpdateServer.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>

#define IR_LED D3  

#ifndef STASSID
#define STASSID "***REMOVED***"
#define STAPSK  "***REMOVED***"
#endif

const char* host = "esp8266-webupdate";
const char* ssid = STASSID;
const char* password = STAPSK;

IRsend irsend(IR_LED);  // Set the GPIO to be used to sending the message.
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

// Poweroff
uint16_t poweroffData[73] = {9018, 4488,  572, 554,  570, 554,  570, 554,  570, 554,  570, 554,  570, 554,  570, 1680,  572, 554,  570, 1680,  572, 1680,  572, 1680,  572, 1680,  572, 1680,  572, 1680,  570, 554,  570, 1680,  572, 554,  570, 1680,  572, 556,  570, 554,  570, 1680,  570, 554,  570, 554,  570, 554,  570, 1682,  570, 554,  570, 1680,  572, 1680,  570, 554,  570, 1682,  570, 1680,  570, 1680,  570, 32730,  78, 7174,  9018, 2242,  570};  // NEC 2FD48B7
uint16_t inputData[71] = {9018, 4488,  570, 554,  570, 554,  570, 554,  570, 554,  570, 554,  570, 552,  570, 1680,  576, 550,  570, 1680,  572, 1680,  572, 1680,  572, 1680,  572, 1678,  572, 1680,  572, 554,  570, 1680,  570, 556,  570, 554,  570, 1680,  572, 554,  570, 1680,  572, 554,  570, 554,  570, 554,  570, 1680,  570, 1680,  570, 554,  570, 1680,  572, 554,  570, 1680,  570, 1680,  572, 1680,  572, 39980,  9044, 2216,  572};  // NEC 2FD28D7

void handleRoot();              // function prototypes for HTTP handlers
void handleLED();
void handleNotFound();

void setup(void) {

  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");
  
  irsend.begin();
  delay(2000);
  irsend.sendRaw(poweroffData, 67, 38);
  delay(2000);
  irsend.sendRaw(inputData, 67, 38);
  
  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }

  MDNS.begin(host);

  httpUpdater.setup(&httpServer);
  httpServer.on("/", HTTP_GET, handleRoot);     // Call the 'handleRoot' function when a client requests URI "/"
  httpServer.on("/power", HTTP_POST, handlePower);
  httpServer.on("/input", HTTP_POST, handleInput);
  httpServer.on("/command", HTTP_POST, handleCommand);
  httpServer.onNotFound(handleNotFound);  
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
}

void loop(void) {
  httpServer.handleClient();
  MDNS.update();
}

void handleRoot() {                         // When URI / is requested, send a web page with a button to toggle the LED
  httpServer.send(200, "text/html", "<form action=\"/input\" method=\"POST\"><input type=\"submit\" value=\"Toggle Input\"></form><form action=\"/power\" method=\"POST\"><input type=\"submit\" value=\"Toggle Power\"></form><form action=\"/command\" method=\"POST\"><input type=\"text\" name=\"data\" placeholder=\"Command\"><input type=\"submit\" value=\"Submit\"></form><a href=\"update\">update</a>");
}

void handlePower() {                          // If a POST request is made to URI /power
  irsend.sendRaw(poweroffData, 67, 38);
  httpServer.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  httpServer.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleInput() {                          // If a POST request is made to URI /power
  irsend.sendRaw(inputData, 67, 38);
  httpServer.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
  httpServer.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleCommand() {                         // If a POST request is made to URI /command
  if( ! httpServer.hasArg("data") 
      || httpServer.arg("data") == NULL) { // If the POST request doesn't have data
    httpServer.send(400, "text/plain", "400: Invalid Request");         // The request is invalid, so send HTTP status 400
    return;
  } else {
    // irsend.sendRaw(httpServer.arg("data").toInt(), 67, 38);
    httpServer.sendHeader("Location","/");        // Add a header to respond with a new location for the browser to go to the home page again
    httpServer.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
  }
}

void handleNotFound(){
  httpServer.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}
