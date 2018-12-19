#include <Adafruit_Sensor.h>
#include <ArduinoJson.h>
#include <DHT.h>

int bpm = 0;
float latitude = 0;
float longtitude = 0;

bool readBPM      = false;
bool readX        = false;
bool finish       = false;
bool finishBPM    = false;
bool startMonitor = false;
int locationCount = 0;
int GPS_x[5];
int GPS_y[5];



void readData()
{
      while(Serial.available())
    {
        if(startMonitor == false)
        {
           char start = Serial.read();
           if(start == 'a' || start == 'b')
           {
                readX = start == 'a' ? true:false;
                startMonitor = true;  
           }
           else if(start == 'c')
           {
               startMonitor = true;
               readBPM = true;
           }
        }
        else if(readBPM == true)
        {
            bpm = Serial.read();
            readBPM = false;
            startMonitor = false;
            finishBPM = true;
        }
        else if(locationCount <= 4)  //1st: signed indicator bit.
        {
           int location = Serial.read();
           //Serial.write(location);
           if(readX == true)
           {
               GPS_x[locationCount] = location;
           }
           else
           {
               GPS_y[locationCount] = location;
               finish = (locationCount == 4)? true : false;  
           }
           locationCount++;
        }
        else
        {
           startMonitor = false;
           locationCount = 0;  
        }
        delay(200);

      if(finish && finishBPM)
      {
          float X = GPS_x[1]*100 + GPS_x[2] + GPS_x[3]*0.01 + GPS_x[4]*0.0001;
          X *= (GPS_x[0] == 0)? 1: (-1);
  
          float Y = GPS_y[1]*100 + GPS_y[2] + GPS_y[3]*0.01 + GPS_y[4]*0.0001;
          Y *= (GPS_y[0] == 0)? 1: (-1);
          latitude = X;
          longtitude = Y;
          
          
          finish = false;   
          finishBPM = false; 
      }
    
    }


}

bool readMessage(int messageId, char *payload)
{

    readData();
    
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.createObject();
    root["deviceId"] = DEVICE_ID;
    root["messageId"] = messageId;
    bool BPMAlert = false;

    root["bpm"] = bpm;
  
    if (bpm > BPM_ALERT) //50
    {
      BPMAlert = true;
      bpm = 0;
    }
    
    if (std::isnan(latitude)){
      root["latitude"] = NULL;
    }
    else
    {
      root["latitude"] = latitude;
    }

    if (std::isnan(longtitude)){
      root["longtitude"] = NULL;
    }
    else
    {
      root["longtitude"] = longtitude;
    }
    root.printTo(payload, MESSAGE_MAX_LEN);
    return BPMAlert;

}

void parseTwinMessage(char *message)
{
    StaticJsonBuffer<MESSAGE_MAX_LEN> jsonBuffer;
    JsonObject &root = jsonBuffer.parseObject(message);
    if (!root.success())
    {
        Serial.printf("Parse %s failed.\r\n", message);
        return;
    }

    if (root["desired"]["interval"].success())
    {
        interval = root["desired"]["interval"];
    }
    else if (root.containsKey("interval"))
    {
        interval = root["interval"];
    }
}
