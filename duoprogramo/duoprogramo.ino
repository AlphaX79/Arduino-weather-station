#include <SoftwareSerial.h>
#include <Wire.h>
#include <AM2320.h>
#include <Adafruit_AM2320.h>
#include <Commands.h>
#include <NovaSDS011.h>

#define RX_PIN 10
#define TX_PIN 11

int  LightPin = 3; //pin 3 połączony z pinem data czujnika naświetlenia
int Trig = 4;  //pin 4 połączony z pinem Trigger czujnika odległości
int Echo = 5;  //pin 5 połączony z pinem Echo czujnika odległości
AM2320 th;
NovaSDS011 pylek;

double getLight()
{
  double duration;
  double frequency;
  duration=pulseIn(LightPin, HIGH);
  frequency = 1 / (duration*0.001);
  return frequency;

}

double getDistance ()
{
  double duration;
  double cm_dist;
  digitalWrite(Trig, LOW);        //upewnienie się, że zaczniemy od LOW
  delayMicroseconds(2);
  digitalWrite(Trig, HIGH);       //ustawienie stanu wysokiego na 10 uS
  delayMicroseconds(10);
  digitalWrite(Trig, LOW);
  digitalWrite(Echo, HIGH); 
  duration = pulseIn(Echo, HIGH);
  cm_dist = duration / 58;                //szerokość odbitego impulsu w uS podzielone przez 58 to odleglosc w cm - patrz dokumentacja
  return cm_dist;
}

double getTemperature(){
  double temperature;
  th.Read();
  temperature=th.t;
  return temperature;
}

double getHumidity(){
  double humidity;
  th.Read();
  humidity=th.h;
  return humidity;
}

double getDust25(){
  float dust25;
  float dust_p25;
  float dust10; 
  pylek.queryData(dust25, dust10);
  dust_p25=float(dust25);
  return dust_p25;
}

double getDust10(){
  float dust25;
  float dust_p10;
  float dust10; 
  pylek.queryData(dust25, dust10);
  dust_p10=float(dust10);
  return dust_p10;
}

void setup()
{
  pinMode(LightPin, INPUT);
  pinMode(Trig, OUTPUT);
  pinMode(Echo, INPUT);
  pylek.begin(RX_PIN, TX_PIN);
  Serial.begin(115200);
  Wire.begin();

}
void loop()
{
  double light;
  double distance;
  double temperature;
  double humidity;
  double dust_p25;
  double dust_p10;

  if (Odczyt.available()){
    Serial.print("Dane z czujnika 1: ");
    Serial.println(DaneZczujnika1); //Odczyt danych ze zmiennej do ktorej wyslalismy dane po odczycie
    Serial.print("Dane z czujnika 2: ");
    Serial.println(OdbieranieDanych.DaneZczujnika2); //Odczyt danych bezposrednio ze struktury wyslanej
    Odczyt.restart();
  }
 
  //Pobieranie danych z czujnikow
  light=getLight();
  distance=getDistance();
  temperature=getTemperature();
  humidity=getHumidity();
  dust_p25=getDust25();
  dust_p10=getDust10();
  delay(1000);

  //Wysylanie zmiennych do portu COM
  Serial.print(light);
  Serial.print(' ');
  Serial.print(distance);
  Serial.print(' ');
  Serial.print(temperature);
  Serial.print(' ');
  Serial.print(humidity);
  Serial.print(' ');
  Serial.print(dust_p25);
  Serial.print(' ');
  Serial.print(dust_p10);
  Serial.println(' ');
  
}
