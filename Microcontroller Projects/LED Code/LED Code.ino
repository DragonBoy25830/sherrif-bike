#include <WiFiNINA.h>
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>

#ifndef PSTR
#define PSTR // Make Arduino Due happy
#endif

#define PIN 6

int powerLED = 7;
int connectionLED = 8;

char ssid[] = ""; 
char pass[] = "";    
int status = WL_IDLE_STATUS;  // connection status
WiFiServer server(80);        // server socket

WiFiClient client = server.available();

// Decode HTTP GET value
String redString = "0";
String greenString = "0";
String blueString = "0";
int pos1 = 0;
int pos2 = 0;
int pos3 = 0;
int pos4 = 0;

// Variable to store the HTTP req  uest
String header;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
    32, 8, PIN,
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT +
        NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
    NEO_GRB + NEO_KHZ800);

int x = matrix.width();
int matrixPass = 0;

const uint16_t rgb_color[] = {
    matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255)};

uint16_t color = rgb_color[0];

// TODO Write a status check function indicating connection status and power
// TODO Create AP access app to remove hard coded credentials (phone app might not be best way)
// TODO Parametrize what text is displayed on the LED panel

void setup()
{
  Serial.begin(9600);

  pinMode(powerLED, OUTPUT);

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(100);
  matrix.setTextColor(rgb_color[0]);

  while (!Serial)
    ;

  enable_WiFi();
  connect_WiFi();

  server.begin();
  printWifiStatus();
}

void loop()
{
  checkStatus();
  drawLED(color);
  client = server.available();

  if (client)
  {
    printWEB();
  }
}

void printWifiStatus()
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void enable_WiFi()
{
  // check for the WiFi module:
  if (WiFi.status() == WL_NO_MODULE)
  {
    Serial.println("Communication with WiFi module failed!");
    // don't continue
    while (true)
      ;
  }

  String fv = WiFi.firmwareVersion();
  if (fv < "1.0.0")
  {
    Serial.println("Please upgrade the firmware");
  }
}

void connect_WiFi()
{
  // attempt to connect to Wifi network:
  while (status != WL_CONNECTED)
  {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }
}

void drawLED(uint16_t inputColor)
{
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(F("HELLO WORLD!"));
  if (--x < -50)
  {
    x = matrix.width();
    if (++matrixPass >= 3)
      matrixPass = 0;
    matrix.setTextColor(inputColor);
  }
  matrix.show();
  delay(100);
}

void printWEB()
{
  if (client)
  {
    currentTime = millis();
    previousTime = currentTime;   // if you get a client,
    Serial.println("new client"); // print a message out the serial port
    String currentLine = "";      // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime)
    { // loop while the client's connected
      currentTime = millis();
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        Serial.write(c);        // print it out the serial monitor
        header += c;
        if (c == '\n')
        { // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {

            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            client.println("<link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\">");
            client.println("<script src=\"https://cdnjs.cloudflare.com/ajax/libs/jscolor/2.0.4/jscolor.min.js\"></script>");
            client.println("</head><body><div class=\"container\"><div class=\"row\"><h1>ESP Color Picker</h1></div>");
            client.println("<a class=\"btn btn-primary btn-lg\" href=\"#\" id=\"change_color\" role=\"button\">Change Color</a> ");
            client.println("<input class=\"jscolor {onFineChange:'update(this)'}\" id=\"rgb\"></div>");
            client.println("<script>function update(picker) {document.getElementById('rgb').innerHTML = Math.round(picker.rgb[0]) + ', ' +  Math.round(picker.rgb[1]) + ', ' + Math.round(picker.rgb[2]);");
            client.println("document.getElementById(\"change_color\").href=\"?r\" + Math.round(picker.rgb[0]) + \"g\" +  Math.round(picker.rgb[1]) + \"b\" + Math.round(picker.rgb[2]) + \"&\";}</script></body></html>");
            // The HTTP response ends with another blank line
            client.println();

            // Request sample: /?r201g32b255&
            // Red = 201 | Green = 32 | Blue = 255

            if (header.indexOf("GET /?r") >= 0)
            {
              pos1 = header.indexOf('r');
              pos2 = header.indexOf('g');
              pos3 = header.indexOf('b');
              pos4 = header.indexOf('&');
              redString = header.substring(pos1 + 1, pos2);
              greenString = header.substring(pos2 + 1, pos3);
              blueString = header.substring(pos3 + 1, pos4);
              Serial.println(redString.toInt());
              Serial.println(greenString.toInt());
              Serial.println(blueString.toInt());

              color = matrix.Color(redString.toInt(), greenString.toInt(), blueString.toInt());
            }
            // Break out of the while loop
            break;
          }
          else
          { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";

    // drawLED(color);
    // Close the connection
    client.stop();

    Serial.println("Client disconnected.");
    Serial.println("");
  }
}

void checkStatus()
{
  if (analogRead(A2) > 400)
    digitalWrite(powerLED, HIGH);

  if (status == WL_CONNECTED)
    digitalWrite(connectionLED, HIGH);
  delay(1000);
}