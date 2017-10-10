/*
 * ESP8266-MQTT-SENSORS for Weather Station (Environmental Monitoring & Automatic Control).
 * 
 * GPIO0    : Data-pin DHT21 sensor.
 * A0       : Analog-pin Soil moisture sensor.
 * GPIO10   : Data-pin DS18B20 sensor.
 * GPIO04   : Output controls the leds.
 * GPIO05   : Output controls the fans.
 * 
 */
// Depends on the following Arduino libraries:
// - Adafruit Unified Sensor Library: https://github.com/adafruit/Adafruit_Sensor
// - DHT Sensor Library: https://github.com/adafruit/DHT-sensor-library

// Set configuration options for WiFi, MQTT, DHT and GPIOs in the following file:
#include "config.h"
#include <String.h>

// WiFi
#include <ESP8266WiFi.h>
WiFiClient espClient;

// MQTT
// - http://pubsubclient.knolleary.net/
#include <PubSubClient.h>
PubSubClient client(espClient);

// SHT10 sensor
// SHT lib for T&RH sensor
//#include <SHT1x.h>
//SHT1x sht1x(CONFIG_SHT_DATA_PIN, CONFIG_SHT_CLOCK_PIN);
//float shtTemp = 0;
//float shtHumi = 0;
//float shtTempCalib = 0;

// DHT sensor
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
// Update this to match your DHT type
#define DHTTYPE DHT21
float humidity = 0.0;
float fahrenheit = 0.0;
float celsius = 0.0;
DHT_Unified dht(CONFIG_DHT_PIN, DHTTYPE);

// DS18B20 sensor
#include <OneWire.h>
#include <DallasTemperature.h>
OneWire oneWire(CONFIG_ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);
float dsTemp = 0;

// Capacitive soil moisture sensor
int smcValue = 0;

// Millis
unsigned long lastSampleTime = 0;
char sampleIdx = 0;

// GPIO's status
boolean ledStatus = false;
boolean fanStatus = false;

// ----------------------------------- GET-PAYLOAD-FROM-MQTT VROKER() ---------------------//
//void receivedMsg(char* topic, unsigned char* payload, unsigned int payloadLength)
void receivedMsg(char* topic, byte* payload, unsigned int payloadLength)
{
    // Connvert payload in char[] to String
    String payloadStr = String((char*)payload);
    // Payload without whitespace
    payloadStr.trim();
    // Get boolean status of the devices
    boolean deviceStatus = payloadStr.toInt();
    // Control the devices: (device_status = 1 --> ON) else OFF
    // Topic: "daothanh/r1/d/01" --> Led
    // Topic: "daothanh/r1/d/02" --> Fan
    if (strcmp(topic, CONFIG_LED_TOPIC) == 0)
    {
      if (deviceStatus != ledStatus)
      {
        ledStatus = deviceStatus;
        // Set led_GPIO HIGH-logic or LOW-logic
        digitalWrite(CONFIG_IO_LED, ledStatus);
      }
      // Publish ledStatus to MQTT broker
      client.publish(CONFIG_LED_STATUS_TOPIC, String(deviceStatus).c_str());
      Serial.println("--------------------");
      Serial.print("LED rec_status: "); Serial.println(deviceStatus);
    }
    else if (strcmp(topic, CONFIG_FAN_TOPIC) == 0)
    {
      if (deviceStatus != fanStatus)
      {
        fanStatus = deviceStatus;
        // Set fan_GPIO HIGH-logic or LOW-logic
        digitalWrite(CONFIG_IO_FAN, fanStatus);
      }
      // Publish fanStatus to MQTT broker
      client.publish(CONFIG_FAN_STATUS_TOPIC, String(deviceStatus).c_str());
      Serial.println("--------------------");
      Serial.print("FAN rec_status: "); Serial.println(deviceStatus);
    }
}
// ----------------------------------- SETUP() - RUN ONCE -----------------------------------//
void setup() {
  // GPIOs as OUTPUT
  pinMode(CONFIG_SIGNAL_LED, OUTPUT);
  //digitalWrite(CONFIG_SIGNAL_LED, HIGH);
  pinMode(CONFIG_IO_LED, OUTPUT);
  pinMode(CONFIG_IO_FAN, OUTPUT);

  // Initilize baud-rate of Serial interface
  Serial.begin(115200);
  // Wifi
  setupWifi();
  // MQTT
  client.setServer(CONFIG_MQTT_HOST, 1883);
  client.setCallback(receivedMsg);
}
// ----------------------------------- SETUP-WIFI() -------------------------------------------//
void setupWifi() {
  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(CONFIG_WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(CONFIG_WIFI_SSID, CONFIG_WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
// ----------------------------------- CONNECT-TO-MQTT BROKER() -------------------------------//
void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect(CONFIG_MQTT_CLIENT_ID, CONFIG_MQTT_USER, CONFIG_MQTT_PASS))
    {
      Serial.println("connected");
      // Subscribe to the topic "baucan/r1/d/#"
      client.subscribe(CONFIG_SUBSCRIBE_TOPIC);
    } else 
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
// ----------------------------------- LOOP() - LOOP FOREVER -----------------------------------//
void loop()
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();
  int k;

  if (currentMillis >= (lastSampleTime + CONFIG_SAMPLE_DELAY))
  {
    lastSampleTime = currentMillis;

    switch(++sampleIdx)
    {
      case 3:
      {
        sampleIdx = 0;
      }
      case 1:
      {
        // Publish deviceStatus to MQTT broker
        client.publish(CONFIG_LED_STATUS_TOPIC, String(ledStatus).c_str());
        delay(50);
        client.publish(CONFIG_FAN_STATUS_TOPIC, String(fanStatus).c_str());

        Serial.println("--------------------");
        Serial.print("LED send_status: "); Serial.println(ledStatus);
        Serial.print("FAN send_status: "); Serial.println(fanStatus);
        break;
      }
      case 2:
      {
        // Read DHT Temperature
        sensors_event_t event;
        for (k = 0; k < 5; k++)
        {
          dht.temperature().getEvent(&event);
          if (isnan(event.temperature))
          {
            celsius = -1;
            //fahrenheit = -1;
          }
          else
          {
            celsius = event.temperature;
            //fahrenheit = cToF(celsius);
            break;
          }
          delay(100);
        }

        // Read DHT Humidity
        for (k = 0; k < 5; k++)
        {
          dht.humidity().getEvent(&event);
          if (isnan(event.relative_humidity))
          {
            //Serial.println("Error reading humidity!");
            humidity = -1;
          }
          else
          {
            humidity = event.relative_humidity;
            break;
          }
          delay(100);
        }
        
        //        // Read SHT1x sensor - AirTemp & AirRH
        //        shtHumi = sht1x.readHumidity();
        //        if ((shtHumi < 0) || (shtHumi > 100)) shtHumi = -1;
        //        // Read temperature as Celsius (the default)
        //        shtTemp = sht1x.readTemperatureC();
        //        //Calibration, only for node 3 
        //        shtTemp = shtTemp + shtTempCalib;
        //        if (shtTemp < 0) shtTemp = -1;
       
        // Read DS18B20 sensor - SoilTemp
        int ds_count = 0;
        do 
        {
          DS18B20.requestTemperatures();
          dsTemp = DS18B20.getTempCByIndex(0);
          ds_count++;
        } 
        while ((dsTemp == 85.0 || dsTemp == (-127.0)) && ds_count < 2);

        // Soil SMC - T
        for (k = 0; k < 3; k++)
        {
          float v = readSoilSensor(CONFIG_IO_SOIL_MOISTURE, CONFIG_SMC_SAMPLE_NUMBER, CONFIG_WATER_VALUE, CONFIG_AIR_VALUE);
          if (v >= 0 && v <= 100)
          {
            smcValue = (int)v;
            break;
          }
          else
          {
            smcValue = -1;
          }
          delay(100);
        }
        
        Serial.println("------------------------------");
        Serial.print("Air Temperature: ");
        Serial.print(celsius);
        Serial.println("*C");
        Serial.print("Air Humidity: ");
        Serial.print(humidity);
        Serial.println("%");

        Serial.print("Soil Temperature: ");
        Serial.print(dsTemp);
        Serial.println(" *C");
        Serial.print("Soil moisture: ");
        Serial.print(smcValue);
        Serial.println("%");
        
        // Build JSON string
        String json = buildJson();
        client.publish(CONFIG_MQTT_TOPIC, json.c_str());
        Serial.println("Sent to MQTT broker!");
        break;
      } // End case
    }   // End switch
  }     // End if-millis
}       // End loop
// ------------------------------- TEMPERATURE CONVERSION ---------------------------------------//
//Celsius to Fahrenheit conversion
float cToF(float cels)
{
  return 1.8 * (float)cels + 32;
}
//Fahrenheit to Celsius conversion
float fToC(float fahr)
{
  return ((float)fahr - 32.0) / 1.8;
}
// ------------------------------- SUB-FUNC_CAPACITOR SOIL MOISTURE SENSOR -----------------------//
float readSoilSensor(int analogPin, int num, int waterVal, int airVal)
{
  int val = 0;
  int sum = 0;
  float SMC = 0;
  for(int i = 0; i < num; i++)
  {
    val = analogRead(analogPin);
    sum += val;
    delay(500);
  }
  val = (int)(sum / num);

  SMC = (1.0 - ((float)val - (float)waterVal) / ((float)airVal - (float)waterVal)) * 100.0;
  if(SMC > 100.0) SMC = 100.0;
  if(SMC < 0.0) SMC = 0.0;
  
  return SMC;
}
// -------------------------------- BUILD JSON -------------------------------------------------//
String buildJson()
{
  String data = "{";
  data += "\"airT\":";
  data += String(celsius).c_str();
  data += ",";
  data += "\"airRH\":";
  data += String(humidity).c_str();
  data += ",";
  data += "\"soilT\":";
  data += String(dsTemp).c_str();
  data += ",";
  data += "\"soilMC\":";
  data += String(smcValue).c_str();
  data += "}";
  return data;
}
// ------------------------------- END --------------------------------------------------------//

