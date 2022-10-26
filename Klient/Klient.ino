/*  Connects to the home WiFi network
 *  Asks some network parameters
 *  Sends and receives message from the server in every 2 seconds
 *  Communicates: wifi_server_01.ino
 */ 
#include <SPI.h>
#include <ESP8266WiFi.h>
#include "SdsDustSensor.h"

int rxPin = D3;
int txPin = D4;
byte ledPin = 2;
char ssid[] = "UPC25457F4";           // SSID of your home WiFi
char pass[] = "ky7jnw3Cvpvx";            // password of your home WiFi
unsigned long askTimer = 0;

SdsDustSensor sds(rxPin, txPin);

IPAddress server(192, 168, 0, 178);       // the fix IP address of the server
WiFiClient client;

void setup() {
 
  Serial.begin(115200);
  sds.begin();   
  WiFi.begin(ssid, pass);             // connects to the WiFi router
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
/*  Serial.println("Connected to wifi");
  Serial.print("Status: "); Serial.println(WiFi.status());    // Network parameters
  Serial.print("IP: ");     Serial.println(WiFi.localIP());
  Serial.print("Subnet: "); Serial.println(WiFi.subnetMask());
  Serial.print("Gateway: "); Serial.println(WiFi.gatewayIP());
  Serial.print("SSID: "); Serial.println(WiFi.SSID());
  Serial.print("Signal: "); Serial.println(WiFi.RSSI());*/
  pinMode(ledPin, OUTPUT);
}

void loop () {

  double pm25;
  double pm10;
  PmResult pm = sds.readPm();
  pm25=pm.pm25;
  pm10=pm.pm10;

  client.connect(server, 80);   // Connection to the server
  digitalWrite(ledPin, LOW);    // to show the communication only (inverted logic)
  Serial.println(".");
  if(pm.isOk()){
  Serial.println(pm25);
  Serial.println(pm10);
  Serial.print(String(pm25) + " " + String(pm10));
  //Serial.println(pm10);
  client.println(pm25);  // sends the message to the server
  client.println(pm10);
  }
  String answer = client.readStringUntil('\r');   // receives the answer from the sever
  Serial.println("from server: " + answer);
  client.flush();
  digitalWrite(ledPin, HIGH);
  delay(2000);                  // client will trigger the communication after two seconds
}
