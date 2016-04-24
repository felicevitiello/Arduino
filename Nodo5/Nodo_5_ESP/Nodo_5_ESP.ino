/**************************************************************************
    Souliss - Hello World for Expressif ESP8266
    
    This is the basic example, create a software push-button on Android
    using SoulissApp (get it from Play Store).  
    
    Load this code on ESP8266 board using the porting of the Arduino core
    for this platform.
        
***************************************************************************/

// Configure the framework
#include "bconf/MCU_ESP8266.h"              // Load the code directly on the ESP8266
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/IPBroadcast.h"
#include "felice_lib/casadomotica.h"        //File con indirizzi, publish subrcibe variabili comuni

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_TSL2561_U.h>


// **** Define the WiFi name and password ****
#define WIFICONF_INSKETCH
#define WiFi_SSID               "WLAN FV"
#define WiFi_Password           "felicesenza"    

// Include framework code and libraries
#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "inshar"
#define AIO_KEY         "7373919d13f62af84b36f4d19b78613fa9e0f84d"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Store the MQTT server, username, and password in flash memory.
// This is required for using the Adafruit MQTT library.
const char MQTT_SERVER[] PROGMEM    = AIO_SERVER;
const char MQTT_USERNAME[] PROGMEM  = AIO_USERNAME;
const char MQTT_PASSWORD[] PROGMEM  = AIO_KEY;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, AIO_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
const char PHOTOCELL_FEED[] PROGMEM = AIO_USERNAME "/feeds/photocell";
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, PHOTOCELL_FEED);

// Setup a feed called 'onoff' for subscribing to changes.
const char ONOFF_FEED[] PROGMEM = AIO_USERNAME "/feeds/casa-io";
Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, ONOFF_FEED);
const char ALARM_ONOFF_FEED[] PROGMEM = AIO_USERNAME "/feeds/alarm-onoff";
Adafruit_MQTT_Subscribe onoffalarm = Adafruit_MQTT_Subscribe(&mqtt, ALARM_ONOFF_FEED);
const char GENERAL_FEED[] PROGMEM = AIO_USERNAME "/feeds/general-feed";
Adafruit_MQTT_Subscribe generalfeed = Adafruit_MQTT_Subscribe(&mqtt, GENERAL_FEED);


/*** All configuration includes should be above this line ***/ 
#include "Souliss.h"



//#define Souliss_T1n_Timed      0x01
#include "DHT.h"
#define in_DHT1 D3
#define DHTTYPE    DHT21  // DHT22 
DHT dht(in_DHT1, DHTTYPE);

// This identify the number of the LED logic
//define degli slot del nodo 0
#define ANTITHEFT 0 
#define L5    5     // luce esterna porta ingresso
#define L6    6     // luce faretti ext
#define Appoggio 14


//Define degli slot di questo nodo

#define DHT1_T    4
#define DHT1_U    6
#define IR1       7
#define TSL1      8
#define BMP1      10

// **** Define here the right pin for your ESP module **** 

#define  in_IR1       D8
#define  in_P1        D1
#define  in_P2        D2

#define gateway_address 51
#define myvNet_address  55


uint8_t ip_address[4]  = {192, 168, 177, myvNet_address}; //indirizzo peer corrente
uint8_t subnet_mask[4] = {255, 255, 255, 0}; //subnet
uint8_t ip_gateway[4]  = {192, 168, 177, 1}; // indirizzo gateway internet
uint8_t gatewaysouliss[4]  = {192, 168, 177, gateway_address}; // indirizzo gateway Souliss
#define myvNet_gateway  gatewaysouliss[3]

Adafruit_TSL2561_Unified tsl = Adafruit_TSL2561_Unified(TSL2561_ADDR_FLOAT, 12345); // Collegare SDA SCL a D1 e D2 per ESP8266
void MQTT_connect();


void setup(){   
  
    Serial.begin(9600);
    Initialize();

    // Connect to the WiFi network and get an address from DHCP
    //GetIPAddress(); 
    SetIPAddress(ip_address, subnet_mask, ip_gateway);
    SetAsGateway(myvNet_gateway); 


    pinMode(in_DHT1, INPUT); 
    pinMode(in_IR1, INPUT); 
    pinMode(in_P1, INPUT); 
    pinMode(in_P2, INPUT); 
    
    Set_T52(DHT1_T);
    Set_T53(DHT1_U);
    Set_T13(IR1);
    Set_T54(TSL1);

    Set_SimpleLight(Appoggio);        // Define a simple LED light logic

    tsl.setIntegrationTime(TSL2561_INTEGRATIONTIME_101MS);
    mqtt.subscribe(&onoffbutton);
}

uint32_t x=0;

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_110ms() {   // We process the logic and relevant input and output every 50 milliseconds
            
            //DigIn(in_IR1, Souliss_T1n_OnCmd, IR1); 
            //Logic_T13(IR1);
            //DigOut(OUT_RELAY1, Souliss_T1n_Coil,IR1);
            if (digitalRead(in_IR1)) mInput(IR1) = Souliss_T1n_OnCmd;
            else mInput(IR1) = Souliss_T1n_OffCmd;

            Logic_T13(IR1);
             
            DigKeepHold(in_P1, Souliss_T1n_OnCmd, Souliss_T1n_ToggleCmd, Appoggio);
            if (mInput(Appoggio) == Souliss_T1n_OnCmd)
             {
              publish(Campanello);
              mInput(Appoggio) =0x0;
             }
             else if (mInput(Appoggio) == Souliss_T1n_ToggleCmd)
             {
              Send(51,L5,Souliss_T1n_Timed+TempoLuciExt);
              Send(51,L6,Souliss_T1n_Timed+TempoLuciExt);
              mInput(Appoggio) =0x0;
             }
            
            //Serial.println("Fine Fast");
        }
        
        FAST_910ms() { 
            /*
            Timer_SimpleLight(RELAY1);
            Logic_PulseOutput(RELAY4);
            DigOut(OUT_RELAY4, Souliss_T1n_Coil,RELAY4);
            */
        }
        FAST_2110ms(){
          MQTT_connect();
            Adafruit_MQTT_Subscribe *subscription;
            if ((subscription = mqtt.readSubscription(300))) {
              if (subscription == &onoffbutton) {
                char *value = (char *)onoffbutton.lastread;
                Serial.print(F("Received: "));
                Serial.println(value);
          
                // Apply message to lamp
                String message = String(value);
                message.trim();
                if (message == "ON") {
                  publish(Notte);
                  Serial.println("Notte");
                  }
                if (message == "OFF") {
                  publish(Giorno);
                  Serial.println("Giorno");
                  }
              }
              if (subscription == &onoffalarm) {
                char *value = (char *)onoffalarm.lastread;
                Serial.print(F("Received: "));
                Serial.println(value);
          
                // Apply message to lamp
                String message = String(value);
                message.trim();
                if (message == "ON") {
                  Send(51,ANTITHEFT,Souliss_T4n_Antitheft);
                  publish(Alarm_Armed);
                   Serial.println("Allarme ON");
                }
                if (message == "OFF") {
                  Send(51,ANTITHEFT,Souliss_T4n_NotArmed);
                  publish(Alarm_Disarmed);
                  Serial.println("Allarme OFF");
                }
                if (subscription == &generalfeed) {
                char *value = (char *)generalfeed.lastread;
                Serial.print(F("General Feed Received: "));
                Serial.println(value);
          
                // Apply message to lamp
                String message = String(value);
                message.trim();
                if (message == "FARETTI_ON") {
                  Send(51,L6,Souliss_T1n_OnCmd);
   
                }
                if (message == "FARETTI_OFF") {
                  Send(51,L6,Souliss_T1n_OffCmd);
                }
                if (message == "SPEGNI-LUCI") {
                 publish(SpegniLuci);
                }
                if (message == "CAMPANELLO") {
                 publish(Campanello);
                }
              }
             }
        }
      }  
      FAST_GatewayComms();    
      }  
        EXECUTESLOW() {
          UPDATESLOW();
              SLOW_10s() {  
                  //Timer_SimpleLight(RELAY3);
                  // Read temperature and humidity from DHT every 10 seconds  
                  float h;
                  // Read temperature as Celsius
                  float t;
                  h = dht.readHumidity();
                  t = dht.readTemperature();
                  // Check if any reads failed and exit early (to try again).
                  if (isnan(h) || isnan(t) ) {
                      h = -999;
                      t = -999;
                  }


                  sensors_event_t event;
                  tsl.getEvent(&event);
                  if (event.light)
                  {
                   mInput(TSL1)=event.light;
                  }
                  
                  ImportAnalog(DHT1_T, &t);
                  ImportAnalog(DHT1_U, &h);
  
                   Logic_T52(DHT1_T);
                   Logic_T53(DHT1_U);
                   Logic_T54(TSL1);
    
              } 
              
        // Here we handle here the communication with Android
                                            
    }
} 

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}

