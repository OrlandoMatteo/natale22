#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

#include <NTPClient.h>
#include <WiFiUdp.h>

#include <OneWire.h>
#include <DallasTemperature.h>

// screen
//  #include <TM74HC595Display.h>
#include "four_bit_display.h"

#define ONE_WIRE_BUS  13

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

FourBitDisplay fbd(16, 4, 5);



// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor
DallasTemperature sensors(&oneWire);

void showTime(int first2, int second2)
{
  char val;
  sprintf(&val, "%02d.%02d", first2, second2);
  // char *val = "12.34";
  short arraySize = strlen(&val);
  char *just = "r";
  fbd.printToDisplay(&val, arraySize, just);
  delay(10);
}
void showTemp(float temp)
{
  char buf[5];
  dtostrf(temp,3, 1, buf);
  buf[4]='C'; //Convert from float to char array
 
  char *val = buf;
  // char *val = "12.34";
  short arraySize = strlen(val);
  char *just = "r";
  fbd.printToDisplay(val, arraySize, just);
  delay(10);
}

void setup()
{
  Serial.begin(115200);
  //pinMode(oneWireBus,INPUT);

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // Uncomment and run it once, if you want to erase all the stored information
   //wifiManager.resetSettings();

  // set custom ip for portal
  // wifiManager.setAPConfig(IPAddress(10,0,1,1), IPAddress(10,0,1,1), IPAddress(255,255,255,0));

  // fetches ssid and pass from eeprom and tries to connect
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP"
  // and goes into a blocking loop awaiting configuration
  wifiManager.autoConnect("AutoConnectAP");
  // or use this for auto generated name ESP + ChipID
  // wifiManager.autoConnect();

  // if you get here you have connected to the WiFi
  Serial.println("Connected.");

  server.begin();

  // NTP
  timeClient.begin();
  timeClient.setTimeOffset(3600);

  // Sensor
  sensors.begin();
  Serial.print("Found ");
Serial.print(sensors.getDeviceCount(), DEC);
Serial.println("devices.");
}

void loop()
{
  WiFiClient client = server.available(); // Listen for incoming clients

  // NTP
  timeClient.update();
  time_t epochTime = timeClient.getEpochTime();
  struct tm *ptm = gmtime((time_t *)&epochTime);
  int monthDay = ptm->tm_mday;
  int currentMonth = ptm->tm_mon + 1;
  Serial.println("date");

  int currentHour = timeClient.getHours();
  int currentMinute = timeClient.getMinutes();
  Serial.println("Time");

  // temp
  sensors.requestTemperatures();
  float temperatureC = sensors.getTempCByIndex(0);
  Serial.println(temperatureC);

  for (int i = 0; i < 500; i++)
  {
    showTime(monthDay, currentMonth);
  }

  for (int j = 0; j < 500; j++)
  {
    showTime(currentHour, currentMinute);
  }
  for (int k = 0; k < 500; k++)
  {

    showTemp(temperatureC);
    
    
  }
  delay(10);
}
