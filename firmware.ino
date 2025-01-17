/* Written by ... */

#include "sb.h"
#include <TinyGsmClient.h>
#include <BlynkSimpleTinyGSM.h>
#include <SoftwareSerial.h>

/* Instance Objects */
BlynkTimer timer;  // Create a BlynkTimer object
SoftwareSerial mySerial(7, 8); //Software Serial for SIM800L RX, TX
TinyGsm modem(mySerial);




bool relay1State = 0;
bool relay2State = 0;

// GSM/GPRS credentials
char apn[] = "web.gprs.mtnnigeria.net";
char ssid[] = "";
char pass[] = "";
bool Connected2Blynk = false;



void setup() {
  Gpio_init();
  // Debug console
  Serial.begin(9600);
  mySerial.begin(9600);
  delay(10);
  
  // Initialize the modem
  Serial.println("Initializing modem...");
  modem.init();

   // Connect to Blynk
  Blynk.begin(BLYNK_AUTH_TOKEN, modem, apn, ssid, pass);
   Blynk.connect(3333);  // timeout set to 10 seconds and then continue without Blynk
  while (Blynk.connect() == false) {
    // Wait until connected
  }
   timer.setInterval(11000L, CheckConnection); // check if still connected every 11 seconds 
  Serial.println("Connected to Blynk server");
}


void loop() {
if(Connected2Blynk){
    Blynk.run(); 
    timer.run();
}

  // Read the state of the buttons
  bool currentButton1State = digitalRead(button1Pin);
  bool currentButton2State = digitalRead(button2Pin);
  bool sensorState = digitalRead(SENSOR);

  // Control indicator LEDs
  if(sensorState) {
    digitalWrite(LEDred, HIGH);
    digitalWrite(LEDgreen, LOW);
    Blynk.virtualWrite(V3, 0); // Send device status to Blynk (0 for off)
  }
  else {
    digitalWrite(LEDred, LOW);
    digitalWrite(LEDgreen, HIGH);
    Blynk.virtualWrite(V3, 1); // Send device status to Blynk (1 for On)
  }

  // Toggle relay 1 if button 1 is pressed
  if (currentButton1State == LOW || relay1State == HIGH) {
    digitalWrite(relay1, HIGH);
    while (digitalRead(SENSOR) == HIGH);
    digitalWrite(relay1, LOW);
    relay1State = LOW;
  }

  // Toggle relay 2 if button 2 is pressed
  if (currentButton2State == LOW || relay2State == HIGH) {
    digitalWrite(relay2, HIGH);
    while (digitalRead(SENSOR) == LOW);
    digitalWrite(relay2, LOW);
    relay2State = LOW;
  }
}

// Blynk virtual pin write handlers
BLYNK_WRITE(V1) {
  relay1State = param.asInt();
}

BLYNK_WRITE(V2) {
  relay2State = param.asInt();
}
void CheckConnection(){
  Connected2Blynk = Blynk.connected();
  if(!Connected2Blynk){
    Serial.println("Not connected to Blynk server"); 
    Blynk.connect(3333);  // timeout set to 10 seconds and then continue without Blynk  
  }
  else{
    Serial.println("Connected to Blynk server");     
  }
   timer.setInterval(11000L, CheckConnection); // check if still connected every 11 seconds 
   return;
}
