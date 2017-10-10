/*
 * This is a sample configuration file for the "mqtt_esp8266_sensors" sensor.
 *
 * Change the settings below and save the file as "config.h"
 * You can then upload the code using the Arduino IDE.
 */

// WiFi
#define CONFIG_WIFI_SSID "Xom nha la"             // SSID of the wi-fi hostpot
#define CONFIG_WIFI_PASS "AgriConnect"            // Passwork of the wi-fi hostpot
//#define CONFIG_WIFI_SSID "FPT-TanPhuc"
//#define CONFIG_WIFI_PASS "01666863453"

// MQTT
#define CONFIG_MQTT_HOST "mqtt.agriconnect.vn"    // MQTT broker
#define CONFIG_MQTT_USER "node"                   // User - connect to MQTT broker
#define CONFIG_MQTT_PASS "654321"                 // Passwork - connect to MQTT broker
#define CONFIG_MQTT_CLIENT_ID "demonode01"          // Must be unique on the MQTT network

// MQTT Topics
#define CONFIG_MQTT_TOPIC "demo/r1/n/01"          // MQTT-Msg-Topic - publish the envi-data to MQTT broker
#define CONFIG_LED_STATUS_TOPIC "demo/r1/ds/01"   // MQTT-Msg-Topic - publish the led-status to MQTT broker
#define CONFIG_FAN_STATUS_TOPIC "demo/r1/ds/02"   // MQTT-Msg-Topic - publish the fan-status to MQTT broker

#define CONFIG_SUBSCRIBE_TOPIC "demo/r1/d/#"      // MQTT-Msg-Topic - subscribe the device-status from MQTT broker
#define CONFIG_LED_TOPIC "demo/r1/d/01"           // MQTT-Msg-Topic - subscribe the led-status from MQTT broker
#define CONFIG_FAN_TOPIC "demo/r1/d/02"           // MQTT-Msg-Topic - subscribe the fan-status from MQTT broker

// DHT sensor
#define CONFIG_DHT_PIN 0                          // Define the DHT type in the code file

// SHT1x sensor
//#define CONFIG_SHT_DATA_PIN 0
//#define CONFIG_SHT_CLOCK_PIN 16

// DS18B20 sensor
#define CONFIG_ONE_WIRE_BUS 10                     // ds_Data pin <-> GPIO10

// Soil moisture sensor
#define CONFIG_IO_SOIL_MOISTURE A0
#define CONFIG_SMC_SAMPLE_NUMBER 5                // Reading 5 samples, after that caculating the average of them
#define CONFIG_SAMPLE_PERIOD 500                  // Delay between 2 readings - in miliseconds
#define CONFIG_AIR_VALUE 434                      // A value between 0 - 1024 when put the SM sensor in the air
#define CONFIG_WATER_VALUE 328                    // A value between 0 - 1024 when put the SM sensor into the water


// Millis
#define CONFIG_SAMPLE_DELAY 10000                 // Milliseconds step - 10 seconds

// SIGNAL LED
#define CONFIG_SIGNAL_LED  16

// GPIOs
#define CONFIG_IO_LED 4                           // GPIO07 of ESP8266 which controls the LED
#define CONFIG_IO_FAN 5                          // GPIO10 of ESP8266 which controls the FAN
