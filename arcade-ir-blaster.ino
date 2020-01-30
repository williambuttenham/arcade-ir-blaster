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
#include <FS.h> // Include the SPIFFS library

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

String getContentType(String filename); // convert the file extension to the MIME type
bool handleFileRead(String path);       // send the right file to the client (if it exists)

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

  SPIFFS.begin();

  httpUpdater.setup(&httpServer);
  httpServer.onNotFound([]() {                              // If the client requests any URI
    if (!handleFileRead(httpServer.uri()))                  // send it if it exists
      httpServer.send(404, "text/plain", "404: Not Found"); // otherwise, respond with a 404 (Not Found) error
  });
  httpServer.begin();

  MDNS.addService("http", "tcp", 80);
  Serial.printf("HTTPUpdateServer ready! Open http://%s.local/update in your browser\n", host);
}

void loop(void)
{
  httpServer.handleClient();
  MDNS.update();
}

String getContentType(String filename)
{
  if (filename.endsWith(".htm"))
    return "text/html";
  else if (filename.endsWith(".html"))
    return "text/html";
  else if (filename.endsWith(".css"))
    return "text/css";
  else if (filename.endsWith(".js"))
    return "application/javascript";
  else if (filename.endsWith(".png"))
    return "image/png";
  else if (filename.endsWith(".gif"))
    return "image/gif";
  else if (filename.endsWith(".jpg"))
    return "image/jpeg";
  else if (filename.endsWith(".ico"))
    return "image/x-icon";
  else if (filename.endsWith(".xml"))
    return "text/xml";
  else if (filename.endsWith(".pdf"))
    return "application/x-pdf";
  else if (filename.endsWith(".zip"))
    return "application/x-zip";
  else if (filename.endsWith(".gz"))
    return "application/x-gzip";
  return "text/plain";
}

bool handleFileRead(String path)
{ // send the right file to the client (if it exists)
  Serial.println("handleFileRead: " + path);
  if (path.endsWith("/"))
    path += "index.html";                    // If a folder is requested, send the index file
  String contentType = getContentType(path); // Get the MIME type
  String pathWithGz = path + ".gz";
  if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path))
  {                                                         // If the file exists, either as a compressed archive, or normal
    if (SPIFFS.exists(pathWithGz))                          // If there's a compressed version available
      path += ".gz";                                        // Use the compressed version
    File file = SPIFFS.open(path, "r");                     // Open the file
    size_t sent = httpServer.streamFile(file, contentType); // Send it to the client
    file.close();                                           // Close the file again
    Serial.println(String("\tSent file: ") + path);
    return true;
  }
  Serial.println(String("\tFile Not Found: ") + path);
  return false; // If the file doesn't exist, return false
}