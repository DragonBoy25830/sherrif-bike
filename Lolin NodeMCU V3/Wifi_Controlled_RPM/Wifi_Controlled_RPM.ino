#include <ESP8266WiFi.h>      // does all Wifi related functionalities
#include <WiFiClient.h>       // required to send request to web browser
#include <ESP8266WebServer.h> // handles HTTP protocls

#include "index.h";

// SSID and Password of your WiFi router where ESP connects
const char *ssid = "";
const char *password = "";

ESP8266WebServer server(80); // Server on port 80
int motor_pin = D6;

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot()
{
  String s = MAIN_page;             // Read HTML contents
  server.send(200, "text/html", s); // Send web page
}

//===============================================================
// This routine is executed when you press submit
//===============================================================
void handleForm()
{
  String input_RPM = server.arg("RPM");

  Serial.print("Input RPM: ");
  Serial.println(input_RPM);

  String s = "<a href='/'> Go Back </a>";
  server.send(200, "text/html", s); // Send web page
}
//==============================================================
//                  SETUP
//==============================================================
void setup()
{
  pinMode(motor_pin, OUTPUT);

  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password); // Connect to your WiFi router

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");

  // If connection successful show IP address in serial monitor
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

  server.on("/", handleRoot);            // Which routine to handle at root location
  server.on("/action_page", handleForm); // form action is handled here

  server.begin(); // Start server
  Serial.println("HTTP server started");
}

//==============================================================
//                     LOOP
//==============================================================
void loop(void)
{
  server.handleClient(); // Handle client requests

  for (int speed = 0; speed <= 1023; speed = speed + 10)
  {
    analogWrite(motor_pin, speed);
    delay(100);
  }
}
