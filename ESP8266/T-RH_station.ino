#include <string.h>
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"

struct ParsedData
{
    String temp;
    String RH;
};

char *ssid = ""; // Insert
char *pass = ""; // Insert

unsigned long channelNum = ; // Insert
char *writeKey = ""; // Insert

WiFiClient client;

void setup() 
{
    Serial.begin(115200);
    Serial.swap(); // use GPIO13/GPIO15 for UART
    ThingSpeak.begin(client);
}

void loop() 
{
    if (WiFi.status() != WL_CONNECTED)
    {
        WiFi.begin(ssid, pass);
        while (WiFi.status() != WL_CONNECTED) 
        {
            delay(500);
        }
    }
    if (Serial.available() > 0)
    {
        String data = Serial.readString();
        struct ParsedData parsedData = parseData(data);
        sendData(parsedData);
    }
}

struct ParsedData parseData(String dataString)
{
    struct ParsedData parsedData;
    
    int commaIndex = dataString.indexOf(",");
    parsedData.temp = dataString.substring(0, commaIndex);
    parsedData.RH = dataString.substring(commaIndex + 1);

    return parsedData;
}

void sendData(struct ParsedData parsedData)
{
    ThingSpeak.writeField(channelNum, 1, parsedData.temp, writeKey);
    delay(20000); // delay for more than 15 sec ThingSpeak delay
    ThingSpeak.writeField(channelNum, 2, parsedData.RH, writeKey);
}
