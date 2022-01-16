#include <TwitchApi.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "CACert.h"
#include <ArduinoJson.h> // This Sketch doesn't technically need this, but the library does so it must be installed.

//------- Replace the following! ------
const char *ssid = "xxxxxxxxxxxxxxxxxxxx";
const char *password = "xxxxxxxxxxxxxxxxxxxx";

// Create a new application on https://dev.twitch.tv/
#define TWITCH_CLIENT_ID "xxxxxxxxxxxxxxxxxxxx"
#define TWITCH_ACCESSTOKEN "xxxxxxxxxxxxxxxxxxxx"





WiFiClientSecure client;
TwitchApi twitch(client, TWITCH_CLIENT_ID, TWITCH_ACCESSTOKEN);

unsigned long delayBetweenRequests = 15000; // Time between requests (15 seconds)
unsigned long requestDueTime = 0;               //time when request due

unsigned long delayBetweenTimeSync = 3600000; // Time between requests (1 hour)
unsigned long timesyncDueTime = 0;

const int maxNames = 5;
String twitchNames[maxNames];

void loadEEPROM()
{
  //Todo: Actually add EEPROM
  twitchNames[0] = "zerator";
}

//This is needed to verify certificates
void setTime()
{
  // Synchronize time using SNTP. This is necessary to verify that
  // the TLS certificates offered by the server are currently valid.
  Serial.print("Setting time using SNTP");
  configTime(8 * 3600, 0, "pool.ntp.org", "time.nist.gov");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

// Load root certificate in DER format into WiFiClientSecure object
void setCertificate()
{
    bool res = client.setCACert_P(caCert, caCertLen);
    if (!res) {
      Serial.println("Failed to load root CA certificate!");
      while (true) {
      yield();
      }
    }
}

void setupWifi()
{
   // Set WiFi to station mode and disconnect from an AP if it was Previously
    // connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    // Attempt to connect to Wifi network:
    Serial.print("Connecting Wifi: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
        digitalWrite(2, LOW); 
    }
    Serial.println("");
    Serial.println("WiFi connected");
    digitalWrite(2, HIGH); 
    Serial.println("IP address: ");
    IPAddress ip = WiFi.localIP();
    Serial.println(ip);
}

void setup()
{
    pinMode(5, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(2, OUTPUT);
    //Define ON LED
    digitalWrite(5, LOW);
    //Define OFF LED
    digitalWrite(4, LOW);
    //Define Activity LED
    digitalWrite(2, LOW);
    Serial.begin(115200);

    //Load EEPROM
    loadEEPROM();

    //Init Wifi connection
    setupWifi();

    //Set Twitch.tv Root Certificate
    setCertificate();

    //Set client timeout
    client.setTimeout(60000);
}

void loop()
{
    //Refresh the clock every hour, not really sure how accurate esp8266 internal clock is
    if (millis() > timesyncDueTime)
    {
      setTime();
      timesyncDueTime = millis() + delayBetweenTimeSync;
    }
    if (millis() > requestDueTime)
    {
        Serial.print("Free Heap: ");
        Serial.println(ESP.getFreeHeap());

        for (int i = 0; i < maxNames; i++) {

          String twitchName = twitchNames[i];
          //Skip empty names
          if(twitchName.length() == 0){continue;}

          Serial.print("Getting Stream info for: ");
          Serial.println(twitchName);
          StreamInfo stream = twitch.getStreamInfo(twitchName.c_str());
          if(!stream.error){
              Serial.println("---------Stream Info ---------");

              Serial.print("Id: ");
              Serial.println(stream.id);

              Serial.print("User Id: ");
              Serial.println(stream.userId);

              Serial.print("User Name: ");
              Serial.println(stream.userName);

              Serial.print("Game Id: ");
              Serial.println(stream.gameId);

              Serial.print("Type: ");
              Serial.println(stream.type);

              Serial.print("Title: ");
              Serial.println(stream.title);

              Serial.print("Viewer Count: ");
              Serial.println(stream.viewerCount);

              Serial.print("Started At: ");
              Serial.println(stream.startedAt);

              Serial.print("Language: ");
              Serial.println(stream.language);

              Serial.print("Thumbnail URL: ");
              Serial.println(stream.thumbnailUrl);
              digitalWrite(5, HIGH);
              digitalWrite(4, LOW);

          } else {
            digitalWrite(5, LOW);
            digitalWrite(4, HIGH);
          }
          
          Serial.println("------------------------");
        }
        requestDueTime = millis() + delayBetweenRequests;
    }
}
