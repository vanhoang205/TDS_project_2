/*
This example shows how to connect to Cayenne using an ESP8266 and send/receive sample data.

The CayenneMQTT Library is required to run this sketch. If you have not already done so you can install it from the Arduino IDE Library Manager.

Steps:
1. If you have not already installed the ESP8266 Board Package install it using the instructions here: https://github.com/esp8266/Arduino#installing-with-boards-manager.
2. Select your ESP8266 board from the Tools menu.
3. Set the Cayenne authentication info to match the authentication info from the Dashboard.
4. Set the network name and password.
5. Compile and upload the sketch.
6. A temporary widget will be automatically generated in the Cayenne Dashboard. To make the widget permanent click the plus sign on the widget.
*/

//#define CAYENNE_DEBUG
#define CAYENNE_PRINT Serial
#include <CayenneMQTTESP8266.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>

WiFiUDP u;
NTPClient n(u, "0.asia.pool.ntp.org", 7*3600);
char ssid[] = "HSD-BKDN";
char password[] = "12345678";
char username[] = "0cab7d20-ecd3-11e8-9c33-75e6b356cec4";
char mqtt_password[] = "c7f6a61776ded5195d0318217edd4c2d35cf4323";
char client_id[] = "906cf790-1700-11e9-a056-c5cffe7f75f9";

WiFiServer server(80);
String rsid;
String rpass;
bool newSSID = false;
bool reconnectAgain = false;
bool checkConnection = true;
int failedClient = false;

int testWifi(void);
void setupAP(void);
void launchWeb();
int mdns1();
String urldecode(const char *src);

const int NUMBER_OF_FIELDS = 5; // how many comma separated fields we expect
int fieldIndex = 0;             // the current field being received
int values[NUMBER_OF_FIELDS];   // array holding values for all the fields


bool checkedTime = true;
void setup()
{

  Serial.begin(115200);
  delay(10);
  if (testWifi()) /*--- if the stored SSID and password connected successfully, exit setup ---*/
  {
    Cayenne.begin(username, mqtt_password, client_id);
    return;
  }
  else /*--- otherwise, set up an access point to input SSID and password  ---*/
  {
    Serial.println("");
    Serial.println("Connect timed out, opening AP");
    setupAP();
  }
  //end setup
  n.begin();
}
void loop()
{
// tu dong connect lai khi mat mang dot ngot
  if (WiFi.status() == WL_CONNECTED)
  {
    checkConnection = true;
    if (reconnectAgain)
    {
      Cayenne.begin(username, mqtt_password, client_id);
      reconnectAgain = false;
    }
  }
  else
  {
    checkConnection = false;
    reconnectAgain = true;
  }
///////////////////////////////////////////////////


  if (Serial.available())
  {
    char ch = Serial.read();
    if (ch >= '0' && ch <= '9') // is this an ascii digit between 0 and 9?
    {
      // yes, accumulate the value
      values[fieldIndex] = (values[fieldIndex] * 10) + (ch - '0');
    }
    else if (ch == ',') // comma is our separator, so move on to the next field
    {
      if (fieldIndex < NUMBER_OF_FIELDS - 1)
        fieldIndex++; // increment field index
    }
    else
    {
      // any character not a digit or comma ends the acquisition of fields
      // in this example it's the newline character sent by the Serial Monitor

      Serial.print(fieldIndex + 1);
      Serial.println(" fields received:");
      for (int i = 0; i <= fieldIndex; i++)
      {
        Serial.println(values[i]);
        values[i] = 0; // set the values to zero, ready for the next message
      }
      fieldIndex = 0; // ready to start over
    }
  }

   static unsigned long timePublic = 0;
    if((unsigned long)(millis()-timePublic) > 5000){
    timePublic = millis();
    n.update();
    if((n.getHours())>6 && (n.getHours())<20){
      checkedTime = true;
    }
    else
    checkedTime = false;
    Serial.println(n.getHours());
    }


    // Serial.print(_tds, 0);
    // Serial.println("ppm");

    // Serial.print(_temp, 0);
    // Serial.println("do c");

    // Serial.print(_DHTtemp, 0);
    // Serial.println("do c - moi truong");

    // Serial.print(_DHThum, 0);
    // Serial.println("do am - moi truong");

    // Serial.print(_statusMotor);
    // Serial.println("trang thai");
    //Cayenne.loop();
  
}

// Default function for sending sensor data at intervals to Cayenne.
// You can also use functions for specific channels, e.g CAYENNE_OUT(1) for sending channel 1 data.
CAYENNE_OUT_DEFAULT()
{
  // Write data to Cayenne here. This example just sends the current uptime in milliseconds on virtual channel 0.
  Cayenne.virtualWrite(0, millis());
  // Some examples of other functions you can use to send data.
  //Cayenne.celsiusWrite(1, 22.0);
  //Cayenne.luxWrite(2, 700);
  //Cayenne.virtualWrite(3, 50, TYPE_PROXIMITY, UNIT_CENTIMETER);
}

// Default function for processing actuator commands from the Cayenne Dashboard.
// You can also use functions for specific channels, e.g CAYENNE_IN(1) for channel 1 commands.
CAYENNE_IN_DEFAULT()
{
  CAYENNE_LOG("Channel %u, value %s", request.channel, getValue.asString());
  //Process message here. If there is an error set an error message using getValue.setError(), e.g getValue.setError("Error message");
}
/*--- auto connect with stored wifi ---*/
  int testWifi(void)    
  {
    int c = 0;
    Serial.println("Waiting for Wifi to connect");  
    while ( c < 20 ) 
    {
      if (WiFi.status() == WL_CONNECTED) 
      {
        // Serial.println("WiFi connected.");
        // Serial.println(WiFi.localIP());

        // lcd.clear();
        // lcd.setCursor(0,0);
        // lcd.print("Wifi connected");
        delay(2000);
        return(1); 
      }      
      delay(500);
      Serial.print(WiFi.status());    
      c++;
    }
    return(0);
  } //end testwifi
void setupAP(void)
{
  WiFi.mode(WIFI_STA); //mode STA
  WiFi.disconnect();   //disconnect to scan wifi
  delay(100);

  Serial.println("");
  delay(100);
  WiFi.softAP(ssid, password); //change to AP mode with AP ssid and APpass
  Serial.println("softAP");
  Serial.println("");
  // lcd.clear();
  // lcd.setCursor(0,0);
  // lcd.print("Connect HSD-BKDN");
  // lcd.setCursor(0,1);
  // lcd.print("192.168.4.1");
  launchWeb(); //?
}

void launchWeb()
{
  Serial.println("");
  Serial.println(WiFi.softAPIP());
  server.begin(); // Start the server
  Serial.println("Server started");
  int b = 20;
  int c = 0;
  while (b == 20)
  {
    b = mdns1(); //mdns1(function: web interface, read local IP SSID, Pass)
    /*--- If a new SSID and Password were sent, close the AP, and connect to local WIFI ---*/
    if (newSSID == true)
    {
      newSSID = false;
      /*--- convert SSID and Password string to char ---*/
      char ssid[rsid.length()];
      rsid.toCharArray(ssid, rsid.length());
      char pass[rpass.length()];
      rpass.toCharArray(pass, rpass.length());

      Serial.println("Connecting to local Wifi");
      delay(500);
      WiFi.softAPdisconnect(true); //disconnet APmode
      delay(500);
      WiFi.mode(WIFI_STA);    //change to STA mode
      WiFi.begin(ssid, pass); //connect to local wifi
      delay(1000);
      ///set -> check -> set
      // lcd.clear();
      // lcd.setCursor(0,0);
      // lcd.print("Connecting....");
      if (testWifi()) //test connect
      {
        Cayenne.begin(username, mqtt_password, client_id);
        return;
      }
      else //if wrong ssid or pass
      {
        // Serial.println("");
        // Serial.println("New SSID or Password failed. Reconnect to server, and try again.");
        // lcd.clear();
        // lcd.setCursor(0,0);
        // lcd.print("SSID,Pass failed");
        delay(2000);
        failedClient = true;
        setupAP();
        return;
      }
    }
  }
}

int mdns1()
{
  // Check for any mDNS queries and send responses
  // Check if a client has connected                    //server mode
  WiFiClient client = server.available(); //check client
  if (!client || (failedClient == true))
  {
    failedClient = false;
    return (20);
  }
  Serial.println("");
  Serial.println("New client");

  // Wait for data from client to become available
  while (client.connected() && !client.available())
  {
    delay(1);
  }

  // Read the first line of HTTP request
  String req = client.readStringUntil('\r');

  // First line of HTTP request looks like "GET /path HTTP/1.1"
  // Retrieve the "/path" part by finding the spaces
  int addr_start = req.indexOf(' ');
  int addr_end = req.indexOf(' ', addr_start + 1);
  if (addr_start == -1 || addr_end == -1)
  {
    Serial.print("Invalid request: ");
    Serial.println(req);
    return (20);
  }
  req = req.substring(addr_start + 1, addr_end);
  Serial.print("Request: ");
  Serial.println(req);
  client.flush();
  String s;
  if (req == "/")
  {
    IPAddress ip = WiFi.softAPIP();
    String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
    s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>HSD-BKDN-ESP8266<br/>";
    s += "IP: ";
    s += ipStr;
    s += "<p>";
    //s += st;
    s += "<form method='get' action='a'>";
    s += "<table><tr><td>SSID:</td><td><input name='ssid' length=32></td> </tr><tr><td>PASS:</td><td><input name='pass' length=64></td> </tr></table>";
    s += "<input type='submit' style='left: 160px; position: relative;'></form>";
    s += "</html>\r\n\r\n";
    Serial.println("Sending 200");
  }
  else if (req.startsWith("/a?ssid="))
  {
    newSSID = true;
    String qsid;                                                  //WiFi SSID
    qsid = urldecode(req.substring(8, req.indexOf('&')).c_str()); //correct coding for spaces as "+"
    Serial.println(qsid);
    rsid = qsid;

    String qpass;                                                                         //Wifi Password
    qpass = urldecode(req.substring(req.indexOf('&') + 6, req.lastIndexOf(' ')).c_str()); //correct for coding spaces as "+"
    Serial.println(qpass);
    rpass = qpass;

    s = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>HSD-BKDN-ESP8266";
    s += "<p> New SSID and Password accepted</html>\r\n\r\n";
  }
  else
  {
    s = "HTTP/1.1 404 Not Found\r\n\r\n";
    Serial.println("Sending 404");
  }
  client.print(s);
  Serial.println("Done with client");
  return (20);
}

String urldecode(const char *src)
{
  String decoded = "";
  char a, b;
  while (*src)
  {
    if ((*src == '%') && ((a = src[1]) && (b = src[2])) && (isxdigit(a) && isxdigit(b)))
    {
      if (a >= 'a')
        a -= 'a' - 'A';
      if (a >= 'A')
        a -= ('A' - 10);
      else
        a -= '0';
      if (b >= 'a')
        b -= 'a' - 'A';
      if (b >= 'A')
        b -= ('A' - 10);
      else
        b -= '0';
      decoded += char(16 * a + b);
      src += 3;
    }
    else if (*src == '+')
    {
      decoded += ' ';
      *src++;
    }
    else
    {
      decoded += *src;
      *src++;
    }
  }
  decoded += '\0';
  return decoded;
}