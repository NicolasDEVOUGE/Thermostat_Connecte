/**

Programme de thermostat connecté sur ESP8266 (ici Wemos D1 mini pro)

V1.0 05/2019
Nicolas DEVOUGE 

Cette version du programme de thermostat fonctionne avec Blynk en GUI sur téléphone portable.

Les varibales à modifier sont celles dans le bloc ### A modifier ###

**/


//### A modifier ###
char auth[] = "***"; //Token Blynk  (donnée par votre appli)
char ssid[] = "***"; // SSID de votre wifi
char pass[] = "***"; // Clef WPA2 de votre wifi
const int relayPin = D3; //Pin pour Relai, moi j'ai pris la D3, car D1 est utilisée par la sonde de température
int32 temp_voulue = 18; // Température de consigne au démarrage de l'ESP8266
unsigned long interval = 10000L; // Interval de rafraichssement entre 2 mesures, en ms , ici 10s
///### A modifier ###

// Blynk
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp8266.h>
BlynkTimer timer;
WidgetLED led3(V3); //Numéro de led virtuelle , doit correspondre sur l'appli
//HP303B capteur température
#include <LOLIN_HP303B.h>
LOLIN_HP303B HP303B;
//Timer
unsigned long previousMillis=0 ;
//Wifi
#include <ESP8266WiFi.h>
// Config du relais
int relayState = LOW; //Relais coupé au lancement
int val = 0;      // variable to store the read value
// Variable globale pour température de base
int32_t temperature;
// Config du WidgetLed pour etat relais
void buttonLedWidget()
{
   // If state has changed...
  if (relayState == HIGH) {
      led3.on();
    } else {
      led3.off();
    }
}
//Setup
void setup()
{
  // Debug console
  Serial.begin(115200);
  while (!Serial)
    ;
  //Address of the HP303B (0x77 or 0x76)
  HP303B.begin(); // I2C address = 0x77
  Blynk.config(auth, "192.168.250.21", 8080);
  Blynk.connect();
  timer.setInterval(1500L, buttonLedWidget);
  pinMode(relayPin, OUTPUT);
}
// Recuperation de la valeur de temperature_voulue depuis Blynk (slider)
BLYNK_WRITE(V10) 
{
  temp_voulue = param.asInt(); // assigning incoming value from pin V10 to a variable
  // process received value
  Serial.println((String)"temperature voulue :"+temp_voulue+" °C");
}
// Fonctione principale, on compare la temp de consigne à celle mesurée par la sonde
void test_temp()
{
  if (temperature < temp_voulue) {
    // on active le relai
    relayState = HIGH;
    val = digitalRead(relayPin);
    Serial.println(val);
    if (val = !relayState) {
    digitalWrite(relayPin,relayState); 
    Blynk.virtualWrite(V6, relayState);
    }
    digitalWrite(relayPin,relayState); 
    Serial.println((String)"temperature relevée :"+temperature+"°C < température voulue : "+temp_voulue+"°C, on chauffe !");

  }//Sinon on active pas
  else {
    relayState = LOW;
    val = digitalRead(relayPin);
    if (val = !relayState) {
    digitalWrite(relayPin, relayState);
    Blynk.virtualWrite(V6, relayState);
    }
    Serial.println((String)"temperature relevée :"+temperature+"°C > température voulue : "+temp_voulue+"°C, on ne chauffe pas !");
    }  
}
//Loop
void loop()
{
  Blynk.run();
  int16_t oversampling = 7; //Parametre de la lib HP303B
  int16_t ret; //Parametre de la lib HP303B
  Serial.println();
  ret = HP303B.measureTempOnce(temperature, oversampling);
    if (ret != 0)
    {
        Serial.print("FAIL! ret = ");
        Serial.println(ret);
    }
    else
    { 
      Blynk.virtualWrite(V5, temperature);
      
 if( millis() - previousMillis >= interval) {
    previousMillis = millis();   
    test_temp() ;
 }
timer.run();
}

}
