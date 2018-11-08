/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set a GPIO pin depending on the request
      http://server_ip/servo/right will set the servo to 0,
      http://server_ip/servo/left will set the servo to 180
    server_ip is the IP address of the ESP8266 module, will be
    printed to Serial when the module is connected.
*/

#include <ESP8266WiFi.h>//Wifi Library
#include <Servo.h>//Servo Library

const char* ssid = "CUSD-Wifi";//WiFi Network name to connect to
const char* password = "";//Password of wifi

WiFiServer server(80);//Create server object on port 80(default)

Servo frontDoorLock;// create servo object to control a servo
int frontDoorPin = 0;//Set the pin for the front door lock to 0

void setup() {
  
  Serial.begin(115200);
  delay(10);
  frontDoorLock.attach(frontDoorPin);  // attaches the servo on GIO2 to the servo object

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);//prints the WiFi network we are connecting to

  WiFi.mode(WIFI_STA);//Set the wifi mode
  WiFi.begin(ssid, password);//Connect to the network with Network Name: ssid, Password: password

  while (WiFi.status() != WL_CONNECTED) {//Wait for connection
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

 
  server.begin();//Start the server
  Serial.println("Server started");

  Serial.print("http://");
  Serial.print(WiFi.localIP());//Print the IP address
}

void loop() {
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // Wait until the client sends some data
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val;
  if (req.indexOf("/servo/left") != -1) {//If the url being visited is http://(IP Address)/servo/left
    val = 0;
  } else if (req.indexOf("/servo/right") != -1) {//If the url being visited is http://(IP Address)/servo/right
    val = 1;
  } else if (req.indexOf("/") != -1) {//If the url being visited is http://(IP Address)/
    
  }else {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  int pos = 180*val;//So if the url was /left, val is 0 and the position becomes 180*0 which is 0, but if the url is /right then the servo will go to position 180*1 which is 180
  frontDoorLock.write(pos);//Make the servo go to pos

  client.flush();

  // Prepare the response
  String webPage = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n<div><h1>Servo Status: ";
  webPage += (val) ? "left" : "right";
  webPage += "</h1></div><a href='/servo/left'>Left</a><a href='/servo/right'>Right</a>";
  webPage += "</html>\n";

  client.print(webPage);//Print out the html
  delay(1);
  Serial.println("Client disonnected");
}
