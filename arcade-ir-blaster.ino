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
#define STAPSK "***REMOVED***"
#endif

#define IR_BPlus 0xFF3AC5  //
#define IR_BMinus 0xFFBA45 //
#define IR_ON 0xFF827D     //
#define IR_OFF 0xFF02FD    //
#define IR_R 0xFF1AE5      //
#define IR_G 0xFF9A65      //
#define IR_B 0xFFA25D      //
#define IR_W 0xFF22DD      //
#define IR_B1 0xFF2AD5     //
#define IR_B2 0xFFAA55     //
#define IR_B3 0xFF926D     //
#define IR_B4 0xFF12ED     //
#define IR_B5 0xFF0AF5     //
#define IR_B6 0xFF8A75     //
#define IR_B7 0xFFB24D     //
#define IR_B8 0xFF32CD     //
#define IR_B9 0xFF38C7     //
#define IR_B10 0xFFB847    //
#define IR_B11 0xFF7887    //
#define IR_B12 0xFFF807    //
#define IR_B13 0xFF18E7    //
#define IR_B14 0xFF9867    //
#define IR_B15 0xFF58A7    //
#define IR_B16 0xFFD827    //
#define IR_UPR 0xFF28D7    //
#define IR_UPG 0xFFA857    //
#define IR_UPB 0xFF6897    //
#define IR_QUICK 0xFFE817  //
#define IR_DOWNR 0xFF08F7  //
#define IR_DOWNG 0xFF8877  //
#define IR_DOWNB 0xFF48B7  //
#define IR_SLOW 0xFFC837   //
#define IR_DIY1 0xFF30CF   //
#define IR_DIY2 0xFFB04F   //
#define IR_DIY3 0xFF708F   //
#define IR_AUTO 0xFFF00F   //
#define IR_DIY4 0xFF10EF   //
#define IR_DIY5 0xFF906F   //
#define IR_DIY6 0xFF50AF   //
#define IR_FLASH 0xFFD02F  //
#define IR_JUMP3 0xFF20DF  //
#define IR_JUMP7 0xFFA05F  //
#define IR_FADE3 0xFF609F  //
#define IR_FADE7 0xFFE01F  //

const char *host = "esp8266-webupdate";
const char *ssid = STASSID;
const char *password = STAPSK;

IRsend irsend(IR_LED); // Set the GPIO to be used to sending the message.
ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

void handleRoot(); // function prototypes for HTTP handlers
void handleLED();
void handleNotFound();

void setup(void)
{

  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");

  irsend.begin();
  delay(2000);
  irsend.sendNEC(0x2FD48B7);
  delay(2000);
  irsend.sendNEC(0x2FD28D7);
  delay(2000);
  irsend.sendNEC(IR_OFF);

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid, password);

  while (WiFi.waitForConnectResult() != WL_CONNECTED)
  {
    WiFi.begin(ssid, password);
    Serial.println("WiFi failed, retrying.");
  }

  MDNS.begin(host);

  httpUpdater.setup(&httpServer);
  httpServer.on("/", HTTP_GET, handleRoot); // Call the 'handleRoot' function when a client requests URI "/"
  httpServer.on("/power", HTTP_POST, handlePower);
  httpServer.on("/input", HTTP_POST, handleInput);
  httpServer.on("/command", HTTP_POST, handleCommand);
  httpServer.onNotFound(handleNotFound);
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
}

void loop(void)
{
  httpServer.handleClient();
  MDNS.update();
}

void handleRoot()
{ // When URI / is requested, send a web page with a button to toggle the LED
  httpServer.send(200, "text/html", "<form action=\"/input\" method=\"POST\"><input type=\"submit\" value=\"Toggle Input\"></form><form action=\"/power\" method=\"POST\"><input type=\"submit\" value=\"Toggle Power\"></form><form action=\"/command\" method=\"POST\"><input type=\"text\" name=\"data\" placeholder=\"Command\"><input type=\"submit\" value=\"Submit\"></form><a href=\"update\">update</a>");
}

void handlePower()
{ // If a POST request is made to URI /power
  irsend.sendNEC(0x2FD48B7);
  httpServer.sendHeader("Location", "/"); // Add a header to respond with a new location for the browser to go to the home page again
  httpServer.send(303);                   // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleInput()
{
  irsend.sendNEC(0x2FD28D7);
  httpServer.sendHeader("Location", "/");
  httpServer.send(303);
}

void handleCommand()
{
  if (!httpServer.hasArg("data") || httpServer.arg("data") == NULL)
  {                                                             // If the POST request doesn't have data
    httpServer.send(400, "text/plain", "400: Invalid Request"); // The request is invalid, so send HTTP status 400
    return;
  }
  else
  {
    if (httpServer.arg("data") == "1")
    {
      irsend.sendNEC(IR_OFF);
      Serial.println("test");
    }

    if (httpServer.arg("data") == "2")
      irsend.sendNEC(0x2FD40BF);

    httpServer.sendHeader("Location", "/");
    httpServer.send(303);
  }
}

void handleNotFound()
{
  httpServer.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}