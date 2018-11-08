/*
    This sketch demonstrates how to set up a simple HTTP-like server.
    The server will set the servo depending on the request
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
  Serial.println("Server started");//Prints out on serial moniter

  Serial.print("http://");
  Serial.print(WiFi.localIP());//Print the IP address link
}

void loop() {

  WiFiClient client = server.available();
  if (!client) {//If the client fails to initialize
    return;//start over
  }

  Serial.println("new client");
  while (!client.available()) {
    delay(1);//Wait for a client to connect
  }

  String req = client.readStringUntil('\r');//Start reading the clients request
  Serial.println(req);//print out the request
  client.flush();//clear it to save memory

  int val;
  if (req.indexOf("/servo/left") != -1)//If the url being visited is http://(IP Address)/servo/left 
  {
    pos = 0;//set servo to 0
  } 
  else if (req.indexOf("/servo/right") != -1)//If the url being visited is http://(IP Address)/servo/right
  {
    pos = 180;//set servo to 180
  } 
  else if (req.indexOf("/") != -1)//If the url being visited is http://(IP Address)/
  {
    //do nothing just print the page
  }
  else 
  {
    Serial.println("invalid request");
    client.stop();
    return;
  }

  frontDoorLock.write(pos);//Make the servo go to position "pos"
  client.flush();

  //Prepare the webpage
  String webPage = "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html>\r\n<div><h1>Servo Status: ";
  webPage += (val) ? "left" : "right";
  webPage += "</h1></div><a href='/servo/left'>Left</a><a href='/servo/right'>Right</a>";
  webPage += "</html>\n";

  client.print(webPage);//Print out the html
  delay(1);
  Serial.println("Client disonnected");
}
