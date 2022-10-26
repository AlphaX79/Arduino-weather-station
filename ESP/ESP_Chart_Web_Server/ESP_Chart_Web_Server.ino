/*-- Rui Santos - Complete project details at https://RandomNerdTutorials.com

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files.
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.*/

  #include <Arduino.h>
  #include <ESP8266WiFi.h>
  #include <Hash.h>
  #include <ESPAsyncTCP.h>
  #include <ESPAsyncWebServer.h>
  #include <FS.h>
  #include <Wire.h>
  #include <LittleFS.h>
  #include <SPI.h>
  #include <espnow.h>
  //#include <WiFi.h>
  #include <Timers.h>
  
uint8_t broadcastAddress[] = {0xCC, 0x50, 0xE3, 0x56, 0x70, 0xB6}; //CC:50:E3:56:70:B6

//Zmienne globalne dostepne dla kazdej funkcji programu
  String light="";
  String temperature="";
  String humidity="";
  String dist="";
  String dust25="";
  String dust10="";


const char* ssid = "XXXXXX";
const char* password = "xxxxxx";

// Tworzenie serwera web na porcie 80
AsyncWebServer server(80);
//WiFiServer serwer(120);


Timer Odczyt;

//Odbierane danych

//Dwie zmienne w których będziemy trzymali dane odebrane
    int DaneZczujnika1;
    int DaneZczujnika2;

typedef struct struct_message {
    int DaneZczujnika1;
    int DaneZczujnika2;
} struct_message;
struct_message OdbieranieDanych; //nazwa struktury danych do której odbieramy dane


//Ta czesc się wykonuje gdy nasz odbiornik odbiera jakieś dane (można dać różne adresy MAC by oddzielić poszczególne nadajniki
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
  memcpy(&OdbieranieDanych, incomingData, sizeof(OdbieranieDanych));
//Wklepujemy odebrane dane do naszych zmiennych gdzie możemy sobie wygodnie je wyświetlić na LCD czy dalej obrabiać.
  DaneZczujnika1 = OdbieranieDanych.DaneZczujnika1;
  DaneZczujnika2 = OdbieranieDanych.DaneZczujnika2;
  Serial.println("Odebralem Dane !");
}

void setup(){
  //Odczytywanie magistrali - z niej pobierane sa odczyty
  Serial.begin(115200);
  Serial.println("Logowanie");
    
    WiFi.mode(WIFI_STA);
  if (esp_now_init() != 0) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

//Inicjowanie ESPnow
  /*esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  if (esp_now_add_peer(&peerInfo) != 0){
    Serial.println("Failed to add peer");
    return;
  }*/
  esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
  esp_now_register_recv_cb(OnDataRecv);

  Odczyt.begin(SECS(2)); // co ile czasu chcemy wyswietlac dane w serial porcie
  
  // Inicjalizacja SPIFFS w celu wgrania pliku html na mikrokontroller
  if(!SPIFFS.begin()){
    Serial.println("Blad polaczenia z SPIFFS");
  }

  IPAddress ip(192, 168, 0, 178);            // IP address of the server
  IPAddress gateway(192,168,0,1);           // gateway of your network
  IPAddress subnet(255,255,255,0);          // subnet mask of your network

  // Laczenie sie z siecia Wi-Fi
  WiFi.config(ip, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Logowanie do sieci trwa...");
  }
  
  if(WiFi.status()==WL_CONNECTED)
  {Serial.println("Zalogowano");
   Serial.println(WiFi.localIP());   //You can get IP address assigned to ESP
  }

  //Odczyt zapisanej w pamieci sterownika strony html 
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/index.html");
  });

  //Podstrony wymagane przez index.html w celu uzyskania odczytow czujnikow
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", temperature.c_str());
  });
  server.on("/dist", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", dist.c_str());
  });
  server.on("/light", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", light.c_str());
  });
    server.on("/humidity", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", humidity.c_str());
  });
      server.on("/dust25", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", dust25.c_str());
  });
      server.on("/dust10", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", dust10.c_str());
  });
  server.begin();
}
 
void loop(){
  String readings[6];
  String clientings[2];
  int data_marker=0;
  String s2;

  //Odczyt RX z magistrali
  String s1=Serial.readStringUntil('\n');

  //Odczyt z klienta
  /*WiFiClient client = serwer.available();
  if (client) {
    if (client.connected()) {
      //digitalWrite(ledPin, LOW);  // to show the communication only (inverted logic)
      s2 = client.readStringUntil('\n');    // receives the message from the client
      Serial.print("From client: "); Serial.println(s2);
      client.flush();
      //digitalWrite(ledPin, HIGH);
    }
    client.stop();                // terminates the connection with the client
  }*/

  if (Odczyt.available()){
    Serial.print("Dane z czujnika 1: ");
    Serial.println(DaneZczujnika1); //Odczyt danych ze zmiennej do ktorej wyslalismy dane po odczycie
    Serial.print("Dane z czujnika 2: ");
    Serial.println(OdbieranieDanych.DaneZczujnika2); //Odczyt danych bezposrednio ze struktury wyslanej
    Odczyt.restart();
  }
  
  //Przydzielanie wartosci odczytow do osobnych zmiennych
  for (int i=0;i<s1.length();i++){
    if(s1[i]!=' ')readings[data_marker]+=s1[i];
    else data_marker+=1;
  }
  
  /*data_marker=0;
  for (int i=0;i<s2.length();i++){
    if(s2[i]!=' ')clientings[data_marker]+=s2[i];
    else data_marker+=1;
  }*/
  
   light=readings[0];
   dist=readings[1];
   temperature=readings[2];
   humidity=readings[3];
   dust25=readings[4];
   dust10=readings[5];
}
  
