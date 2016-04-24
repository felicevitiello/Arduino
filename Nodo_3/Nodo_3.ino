/**************************************************************************
    Souliss - Hello World
    
    This is the basic example, control one LED via a push-button or Android
    using SoulissApp (get it from Play Store).  
    
    Run this code on one of the following boards:
      - Arduino Ethernet (W5100) 
      - Arduino with Ethernet Shield (W5100)
      
    As option you can run the same code on the following, just changing the
    relevant configuration file at begin of the sketch
      - Arduino with W5200 Ethernet Shield
      - Arduino with W5500 Ethernet Shield
        
***************************************************************************/

// Configure the framework
#include "bconf/StandardArduino.h"          // Use a standard Arduino
#include "conf/ethW5100.h"                  // Ethernet through Wiznet W5100
//#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/Webhook.h"                   // Enable DHCP and DNS
#include "felice_lib/casadomotica.h"        //File con indirizzi, publish subrcibe variabili comuni




// Include framework code and libraries
#include <SPI.h>
#include "Souliss.h"

// definisco gli slot associati alle logiche
//Nodo 3

//definisco gli slot
#define ANTITHEFT               0           // This is the memory slot used for the execution of the anti-theft
#define WATCHDOG                1           // This is the memory slot used for the execution of the watchdog
#define L12    2  //luce bagno P1
#define L13    3  //luce studio
#define L10    4  //Ausiliario
#define L11    5  //luce scale
#define O14    6  //Luce Antistudio
#define IR10    7   //ir bagno 
#define IR14    8   //ir scale
#define IR15    9   //ir studio
#define DHT10_T  10  
#define DHT10_U  12
#define DHT13_T  14  
#define DHT13_U  16
#define O15    18 //OPZIONALE


//definisco gli Input

//#define P10    2  
#define P11    6 //ok 
#define P17    2 //ok
#define P18    3  //ok
#define in_IR10   A6  //Prima 0 
#define in_IR14   A7  //Prima 8 
#define in_IR15   A5  //Prima 2
#define in_DHT10  7 //ok
#define in_DHT13  5 //ok


#include "DHT.h"
#define DHTTYPE    DHT11  // DHT22 
DHT dht1(in_DHT10, DHTTYPE);
DHT dht2(in_DHT13, DHTTYPE);

//Definisco gli output
#define O_L12 A0  
#define O_L13 A1 
#define O_L10 A2
#define O_L11 A3
#define O_O14 8// prima A4 
#define O_O15 4 //Prima A5 //Uscita Opzionale


// This sketch will use DHCP, but a generic IP address is defined in case
// the DHCP will fail. Generally this IP address will not be used and doesn't
// need to be used in SoulissApp

IPAddress ip(192, 168, 177, Peer_address_3);
IPAddress gateway(192, 168, 177, ip_gateway_address);
IPAddress subnet(255, 255, 255, 0);


void setup()
{   
    Initialize();
        Ethernet.begin(ip, gateway, subnet);    // If DHCP fails, use a manual address
    

        
//        pinMode(P10, INPUT);                  
//        digitalWrite(P10, HIGH);           //internal pullup resistor   
        pinMode(P11, INPUT);                  
        digitalWrite(P11, HIGH);           //internal pullup resistor   
        pinMode(P17, INPUT);                 
        digitalWrite(P17, HIGH);           //internal pullup resistor   
        pinMode(P18, INPUT);                  
        digitalWrite(P18, HIGH);           //internal pullup resistor   
        
        digitalWrite(in_IR10, LOW);
        pinMode(in_IR10, INPUT);
        digitalWrite(in_IR14, LOW);
        pinMode(in_IR14, INPUT);
        digitalWrite(in_IR15, LOW);
        pinMode(in_IR15, INPUT);
        
        pinMode(in_DHT10, INPUT);
        digitalWrite(in_DHT10, HIGH);
        
        pinMode(in_DHT13, INPUT);
        digitalWrite(in_DHT13, HIGH);
        
        pinMode(O_L10, OUTPUT);                
        pinMode(O_L11, OUTPUT);
        pinMode(O_L12, OUTPUT);
        pinMode(O_L13, OUTPUT);
        pinMode(O_O14, OUTPUT);
        pinMode(O_O15, OUTPUT);
      

        Set_SimpleLight(L10);
        Set_SimpleLight(L11);
        Set_SimpleLight(L12);
        Set_SimpleLight(L13);
        Set_SimpleLight(O14);
        Set_SimpleLight(O15);
        Set_T52(DHT10_T);
        Set_T53(DHT10_U);
        Set_T52(DHT13_T);
        Set_T53(DHT13_U); 
        Set_T42(ANTITHEFT);
        Set_T13(IR10);
        Set_T13(IR14);
        Set_T13(IR15);

}



void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_110ms() {   // We process the logic and relevant input and output every  110 milliseconds

            if (!Alarm_Inserted &&(mOutput(IR10) || mOutput(IR14) || mOutput(IR15))){
              LowDigKeepHold(P17, Souliss_T1n_Timed+TempoScale, Souliss_T1n_ToggleCmd, L11); 
              if (mInput (L11)>Souliss_T1n_Timed && mOutput(L11) == Souliss_T1n_OnCoil) mInput(L11) = Souliss_T1n_ToggleCmd;
              
              if (mInput(L11) == Souliss_T1n_Timed+TempoScale) {
                  publish (LuceScalePT);
                }
              
              LowDigKeepHold(P11, Souliss_T1n_Timed+TempoBagno, Souliss_T1n_ToggleCmd, L12); 
              if (mInput (L12)>Souliss_T1n_Timed && mOutput(L12) == Souliss_T1n_OnCoil) mInput(L12) = Souliss_T1n_ToggleCmd;

              LowDigKeepHold(P18, Souliss_T1n_Timed+TempoCamere, Souliss_T1n_ToggleCmd, L13);
              if (mInput (L13)>Souliss_T1n_Timed && mOutput(L13) == Souliss_T1n_OnCoil) mInput(L13) = Souliss_T1n_ToggleCmd;

              if (!DAY && mOutput(IR14) == 1) 
                mInput(L11) = Souliss_T1n_Timed+TempoScale; //Accendo la  luce Anticamera se e notte e rilevo movimento
              
              mInput(014) =  mInput(L11);
            }
            if(subscribe_s(SpegniLuci))
              {
                mInput(L10)=Souliss_T1n_OffCmd;
                mInput(L11)=Souliss_T1n_OffCmd;
                mInput(L12)=Souliss_T1n_OffCmd;
                mInput(L13)=Souliss_T1n_OffCmd;
                mInput(O14)=Souliss_T1n_OffCmd;
              }
            if(subscribe_s(LuceScalePT))
              {
                mInput(L11)=Souliss_T1n_Timed+TempoScale;
                mInput(O14)=Souliss_T1n_Timed+TempoScale;
              }
            if(subscribe_s(Alarm_Armed)) Alarm_Inserted = true;
            if(subscribe_s(Alarm_Disarmed)) Alarm_Inserted = false;

             IR_AN = analogRead(in_IR10);
            if (IR_AN> IRONOFF) mInput(IR10) = Souliss_T1n_OnCmd;
            else  mInput(IR10) = Souliss_T1n_OffCmd;

            IR_AN = analogRead(in_IR14);
            if (IR_AN> IRONOFF) mInput(IR14) = Souliss_T1n_OnCmd;
            else  mInput(IR14) = Souliss_T1n_OffCmd;

            IR_AN = analogRead(in_IR15);
            if (IR_AN> IRONOFF) mInput(IR15) = Souliss_T1n_OnCmd;
            else  mInput(IR15) = Souliss_T1n_OffCmd;
            
            if (test){
                
              
                if (mOutput(IR10) == 1)  mInput(L12) =Souliss_T1n_OnCmd;
                if (mOutput(IR10) == 0)  mInput(L12) =Souliss_T1n_OffCmd;
                if (mOutput(IR14) == 1)  mInput(L11) =Souliss_T1n_OnCmd;
                if (mOutput(IR14) == 0)  mInput(L11) =Souliss_T1n_OffCmd;
                if (mOutput(IR15) == 1)  mInput(L13) =Souliss_T1n_OnCmd;
                if (mOutput(IR15) == 0)  mInput(L13) =Souliss_T1n_OffCmd;
              }
            
            Logic_T13(IR10);
            Logic_T13(IR14);
            Logic_T13(IR15);
            
            Logic_SimpleLight(L13);
            Logic_SimpleLight(L12);
            Logic_SimpleLight(L10);
            Logic_SimpleLight(L11);
            Logic_SimpleLight(O14);
            Logic_SimpleLight(O15);
            

           
            mOutput(014) =  mOutput(L11);
            DigOut(O_L10, Souliss_T1n_Coil, L10); 
            DigOut(O_L11, Souliss_T1n_Coil, L11); 
            DigOut(O_L12, Souliss_T1n_Coil, L12); 
            DigOut(O_L13, Souliss_T1n_Coil, L13); 
            DigOut(O_O14, Souliss_T1n_Coil, O14);
            DigOut(O_O15, Souliss_T1n_Coil, O15);  
        } 
        FAST_1110ms()   {
           

            if (Alarm_Inserted && (mOutput(IR10) || mOutput(IR14) || mOutput(IR15))) mInput(ANTITHEFT) = Souliss_T4n_Alarm; //Se attivo processo gli IR per allarme
            Logic_T42(ANTITHEFT,gateway_address);
        }
        FAST_2110ms() { 
            mInput(ANTITHEFT) = Watchdog(Peer_address_4, WATCHDOG, Souliss_T4n_Alarm);

            if(memory_map[MaCaco_AUXIN_s + L11] > Souliss_T1n_Timed && mOutput(IR14) == Souliss_T1n_OnCoil)        // se accesa e rileva movimento
              memory_map[MaCaco_AUXIN_s + L11] = Souliss_T1n_Timed + TempoScale; 

            if(memory_map[MaCaco_AUXIN_s + L12] > Souliss_T1n_Timed && mOutput(IR10) == Souliss_T1n_OnCoil)        // se accesa e rileva movimento
            memory_map[MaCaco_AUXIN_s + L12] = Souliss_T1n_Timed + TempoBagno; 

            if(memory_map[MaCaco_AUXIN_s + L13] > Souliss_T1n_Timed && mOutput(IR15) == Souliss_T1n_OnCoil)        // se accesa e rileva movimento
            memory_map[MaCaco_AUXIN_s + L13] = Souliss_T1n_Timed + TempoCamere; 
            
            Timer_SimpleLight(L10);
            Timer_SimpleLight(L11);
            Timer_SimpleLight(O14);

            if(subscribe_s(Giorno))
            {
              DAY = true;
            }
            if(subscribe_s(Notte))
            {
              DAY = false;
            }
          }
              
        // Here we handle here the communication with Android, commands and notification
        // are automatically assigned to MYLEDLOGIC
        FAST_PeerComms();   
   }                                     
        EXECUTESLOW() {
          UPDATESLOW();
              SLOW_10s() {

                  Timer_SimpleLight(L12);
                  Timer_SimpleLight(L13);
  
                  // Read temperature and humidity from DHT every 10 seconds  
                  float h;
                  // Read temperature as Celsius
                  float t;
                  h = dht1.readHumidity();
                  t = dht1.readTemperature();
                  // Check if any reads failed and exit early (to try again).
                  if (isnan(h) || isnan(t) ) {
                      h = -999;
                      t = -999;
                  }
                  ImportAnalog(DHT10_T, &t);
                  ImportAnalog(DHT10_U, &h);
  
                  
                  h = dht2.readHumidity();
                  t = dht2.readTemperature();
                  // Check if any reads failed and exit early (to try again).
                  if (isnan(h) || isnan(t) ) {
                      h = -999;
                      t = -999;
                  }
                  ImportAnalog(DHT13_T, &t);
                  ImportAnalog(DHT13_U, &h);
  
                  
                   Logic_T52(DHT10_T);
                   Logic_T53(DHT10_U);
                    
                   Logic_T52(DHT13_T);
                   Logic_T53(DHT13_U);
    
              } 
      }
    
} 
