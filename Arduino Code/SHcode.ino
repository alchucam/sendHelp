
//calibrate

//starting
bool millisRoll = false;

//LCD

#include "rgb_lcd.h"
rgb_lcd lcd;
//color for basic
const byte colorR = 255;
const byte colorG = 255;
const byte colorB = 0;
//color for dangerous
const byte lcolorR = 255;
const byte lcolorG = 0;
const byte lcolorB = 0;
//color for normal
const byte ncolorR = 135;
const byte ncolorG = 206;
const byte ncolorB = 250;
//color for button
const byte bcolorR = 255;
const byte bcolorG = 178;
const byte bcolorB = 102;
//color for lost connection
const byte ccolorR = 102;
const byte ccolorG = 255;
const byte ccolorB = 102;
bool threetwo = false;
byte cursorX = 0;
//String lineOne = "Blood Type:O Rh+";
//String lineTwo = "Drug Allergy: Hydrocodone ";
String lineOne = "Danger!";
String lineTwo = "Blood Type:O Rh+. Drug Allergy: Hydrocodone. ";
String printlineTwo = lineTwo;

//unsigned long lcdTime = 0;
unsigned long printTimer = 0;

//heart rate MAX30105
#include "MAX30105.h"
#include "heartRate.h"
MAX30105 particleSensor;
const byte RATE_SIZE = 10; //Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE]; //Array of heart rates
byte rateSpot = 0;
unsigned long lastBeat = 0; //Time at which the last beat occurred
float beatsPerMinute;
//int beatAvg;
int beatAvg = 0;
unsigned long delta = 0;
//unsigned long beatAvgCounter = 0;

//GPS
#include <Adafruit_GPS.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(3, 2);
Adafruit_GPS GPS(&mySerial);
int longt[5];
int lat[5];

#define GPSECHO  false //true for debugging.

// this keeps track of whether we're using the interrupt
// off by default!
boolean usingInterrupt = false;
void useInterrupt(boolean); // Func prototype keeps Arduino 0023 happy

// Interrupt is called once a millisecond, looks for any new GPS data, and stores it
SIGNAL(TIMER0_COMPA_vect) {
  char c = GPS.read();
  // if you want to debug, this is a good time to do it!
#ifdef UDR0
  if (GPSECHO)
    if (c) UDR0 = c;  
    // writing direct to UDR0 is much much faster than Serial.print 
    // but only one character can be written at a time. 
#endif
}

uint32_t timer = millis();


//WIFI ESP8266

SoftwareSerial esp(5,4);
unsigned long espTimer = 0;



//Buzzer
#define BUZZER_PIN 8

//Button
//state of button press
//don't need this.
//we have button
bool button_pushed = false;
bool button_print = false;
bool button_cancelling = false;
bool button_cancelled = false;
bool button_pushing = false;


unsigned long buttonTimer3 = 0;


//dangerous situation setup
bool danger = false;
bool connection = false;
bool normal = false;
bool switching = true;
bool noTouch = false;

void setup() {
  //initial setup
  Serial.begin(115200);

  //buuzer setup
  pinMode(BUZZER_PIN, OUTPUT);

  //LCD setup
  lcd.begin(16, 2);
  lcd.setRGB(colorR, colorG, colorB);

  //heartrate sensor setup
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) //Use default I2C port, 400kHz speed
  {
    Serial.println("MAX30105 was not found. Please check wiring/power. ");
    while (1);
  }
  Serial.println("Place your index finger on the sensor with steady pressure.");
  particleSensor.setup(); //Configure sensor with default settings
  particleSensor.setPulseAmplitudeRed(0x0A); //Turn Red LED to low to indicate sensor is running
  particleSensor.setPulseAmplitudeGreen(0); //Turn off Green LED



  //esp setup
  esp.begin(115200);
  delay(50);
  
//  esp.begin(115200);
  //GPS setup
  GPS.begin(9600);
  GPS.sendCommand(PMTK_SET_NMEA_OUTPUT_RMCGGA);
  // Set the update rate
  GPS.sendCommand(PMTK_SET_NMEA_UPDATE_1HZ);   // 1 Hz update rate
  GPS.sendCommand(PGCMD_ANTENNA);
  useInterrupt(true);
//  prepGPS();

  

}
void loop() {


  //button
  int button;
  button = digitalRead(6);
  
  //heartrate
  getBPM();
  //Serial.print("beatAvg");
  //Serial.println(beatAvg);

  //GPS


  //safety measure
  //after millis rollover (After 50 days!); when millis back to 0.
  //if it happens, reset everytimer to 0 
  if (millis() < 100)
  {
    //lcdTime = 0;
    
    buttonTimer3 = 0;

    printTimer = 0;
    lastBeat = 0;
    delta = 0;
    timer = millis(); //GPS timer reset
    Serial.println(millis());
  }

  //button is pressed.
  else if ((button == HIGH) && !button_pushed)
  {
    digitalWrite(BUZZER_PIN, LOW); 
    switching = true;
    button_pushed = true;
    button_cancelled = true;
    printTimer = millis();
  }
  //part 2 of button is pressed
  else if (button_pushed)
  {
    if (button_print) //so to print only once.
    {
      if (button == HIGH && !button_pushing)
      {
        button_pushing = true;
        buttonTimer3 = millis();
      }
      else if (button_pushing && ((millis() - buttonTimer3) > 1000))
      {
        button_pushing = false;
        button_pushed = false;
        button_print = false;
      }
      
    }
    else if (button_cancelled && ((millis() - printTimer) > 1000))
    {
      lcd.clear();
      lcd.setRGB(bcolorR, bcolorG, bcolorB);
      lcd.setCursor(0,0);
      lcd.print("Cancelled");
      lcd.setCursor(0,1);
      lcd.print("Press to restart"); 
      button_cancelled = false;
      button_print = true;      
    }

  }
  else if (switching){
    lcd.clear();
    switching = false;
    Serial.println("switiching");
  }
  else if (!switching)
  {
    if (!connection){
    digitalWrite(BUZZER_PIN, LOW); 
      connectionLCD();   
//      dangerGPS();    
    }
    else if (danger){
      digitalWrite(BUZZER_PIN, HIGH); 
      dangerLCD(); 
      dangerGPS();   
    }
    else if (normal){
      digitalWrite(BUZZER_PIN, LOW); 
      normalLCD();
    }

    //state change to other state
    if ((danger || normal) && noTouch){
      switching = true;
      connection = false;
      normal = false;
      danger = false;
      //digitalWrite(BUZZER_PIN, LOW); 
    }
    else if ((!connection || normal) && (beatAvg > 30)){
      switching = true;
      connection = true;
      normal = false;
      danger = true;
      //digitalWrite(BUZZER_PIN, HIGH); 

    }
    else if ((!connection || danger) && (beatAvg <= 30 && beatAvg > 0)){
      switching = true;
      connection = true;
      normal = true;
      danger = false;
      //digitalWrite(BUZZER_PIN, LOW); 
    }
  }
   
}

  void getBPM()
  {
    long irValue = particleSensor.getIR();
    if (checkForBeat(irValue) == true)
    {
      //We sensed a beat!
      noTouch = false;
      
      
      
      delta = millis() - lastBeat;
      lastBeat = millis();
  
      beatsPerMinute = 60 / (delta / 1000.0);
  
      if (beatsPerMinute < 255 && beatsPerMinute > 20)
      {
        rates[rateSpot++] = (byte)beatsPerMinute; //Store this reading in the array
        rateSpot %= RATE_SIZE; //Wrap variable
  
        //Take average of readings
        beatAvg = 0;
        for (byte x = 0; x < RATE_SIZE ; x++)
          beatAvg += rates[x];
        beatAvg /= RATE_SIZE;
                

      }
    }
    if (irValue < 50000){
      //Serial.println(" No finger?");
      beatAvg = 0;
      //beatAvgCounter = 0;
      rateSpot = 0;
      noTouch = true;
    }
    
  }


  String slideString(String str)
  {
    //String back = str.substring(0,1);
    //str = str.substring(1) + back;
    return str.substring(1) + str.substring(0,1);
  }



  void dangerLCD()
  {
    

    if ((millis() - printTimer) > 500){

//      lcd.clear();
      lcd.setRGB(lcolorR, lcolorG, lcolorB);
      lcd.setCursor(0,0);
      lcd.print(lineOne + " BPM: " + String(beatAvg));
      lcd.setCursor(0,1);
      lineTwo = slideString(lineTwo);
      printlineTwo = lineTwo.substring(0, 16);
      lcd.print(printlineTwo);
      printTimer = millis();
      //lcd.print(printlineTwo);
    }
  }

  

  void normalLCD()
  {
    if ((millis() - printTimer) > 500){
      lcd.setRGB(ncolorR, ncolorG, ncolorB);
      lcd.setCursor(0,0);
      lcd.print("Normal Condition");
      lcd.setCursor(0,1);
      lcd.print("BPM: " + String(beatAvg));
      printTimer = millis();
    }
    
  }
  
  void connectionLCD()
  {
    if ((millis() - printTimer) > 500) {
      lcd.setRGB(ccolorR, ccolorG, ccolorB);
      lcd.setCursor(0,0);
      lcd.print("Lost connection");
      lcd.setCursor(0,1);
      lcd.print("Please reconnect"); 
      printTimer = millis();
    }
  }
  
//GPS functions
void useInterrupt(boolean v) {
  if (v) {
    // Timer0 is already used for millis() - we'll just interrupt somewhere
    // in the middle and call the "Compare A" function above
    OCR0A = 0xAF;
    TIMSK0 |= _BV(OCIE0A);
    usingInterrupt = true;
  } else {
    // do not call the interrupt function COMPA anymore
    TIMSK0 &= ~_BV(OCIE0A);
    usingInterrupt = false;
  }
}



void dangerGPS()
{

       mySerial.listen();
      
      if (GPS.newNMEAreceived()) {
        if (!GPS.parse(GPS.lastNMEA()))   // this also sets the newNMEAreceived() flag to false
          return;  // we can fail to parse a sentence in which case we should just wait for another
      }

  
    if (((millis()-espTimer) > 6000) || (espTimer == 0)){
 
      
      Serial.print(GPS.latitudeDegrees, 4);
      Serial.print(", "); 
      Serial.println(GPS.longitudeDegrees, 4);
      

      
      float latOne = GPS.latitudeDegrees;
      
      if (latOne > 0){
        lat[0] = 0;
      }
      else{
        lat[0] = 1;
        latOne = -latOne;
      }
      
      long latOnez = (long)(latOne * 10000);

      
      for (int i = 4; i >= 1; i--){
        int latt = (int)(latOnez % 100); //last 2 digits.
        lat[i] = latt;
        latOnez /=100;
      }


      float lt = GPS.longitudeDegrees;

      if (lt > 0)
      {
        longt[0] = 0;
      }
      else{
        longt[0] = 1;
        lt = -lt;
      }
      
      long ltz = (long)(lt * 10000);


      for (int i = 4; i >= 1; i--){
        int ltt = (int)(ltz % 100); //last 2 digits.
        longt[i] = ltt;
        ltz = ltz /100;
      }

    

    esp.listen();
    esp.write('a');
    for (int i = 0; i < 5; i++){
      esp.write(lat[i]);
    }


    esp.write('b');
    for (int i = 0; i < 5; i++){
      esp.write(longt[i]);
    }

    esp.write('c');
    esp.write(beatAvg);
    Serial.println(beatAvg);
    espTimer = millis();
    
    }//endif


}
