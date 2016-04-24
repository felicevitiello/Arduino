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

//definisco gli slot
#define ANTITHEFT               0           // This is the memory slot used for the execution of the anti-theft
#define WATCHDOG                1           // This is the memory slot used for the execution of the watchdog
#define L14    2  //Luce Anticamera
#define L16    3  //Luce Cameretta
#define L17    4  //Luce  Camera da Letto
#define L19    5   //Luce Sgabuzzino
#define IR11   6  //IR Cameretta
#define IR12   7  //IR Anticamere 
#define IR13   8  //IR Matrimoniale
#define DHT11_T 9
#define DHT11_U 11
#define DHT12_T 13
#define DHT12_U 15

//definisco gli Input
#define P12    7    //Prima 0  
#define P13    8    //Prima 1 
#define P16    3
#define P15    5
#define P14    6
#define in_IR11 A6    //Prima 7
#define in_IR12 A7    //Prima 8
#define in_DHT11  2
#define in_IR13  A0
#define in_DHT12 A1


//Definisco gli output
#define O_L14  A2  
#define O_L16  A3 
#define O_L17  A4
#define O_L19  A5


#include "DHT.h"
#define DHTTYPE    DHT11  // DHT22 
DHT dht1(in_DHT11, DHT22);
DHT dht2(in_DHT12, DHTTYPE);

// This sketch will use DHCP, but a generic IP address is defined in case
// the DHCP will fail. Generally this IP address will not be used and doesn't
// need to be used in SoulissApp


IPAddress ip(192, 168, 177, Peer_address_4);
IPAddress gateway(192, 168, 177, ip_gateway_address);
IPAddress subnet(255, 255, 255, 0);


void setup()
{   
    Initialize();

    // Network configuration    
    Ethernet.begin(ip, gateway, subnet);    // If DHCP fails, use a manual address
    
    // We connect a pushbutton between 5V and pin2 with a pulldown resistor 
    // between pin2 and GND, the LED is connected to pin9 with a resistor to
    // limit the current amount
    
    //Definisco i PIN d'ingresso pulsanti, HW pulldown resistor inferiore a 50k



        digitalWrite(O_L14, HIGH);        
        digitalWrite(O_L16, HIGH);        
        digitalWrite(O_L17, HIGH);
        digitalWrite(O_L19, HIGH);
        pinMode(O_L14, OUTPUT);                 // Power the LED
        pinMode(O_L16, OUTPUT);
        pinMode(O_L17, OUTPUT);
        pinMode(O_L19, OUTPUT);
        
        
        pinMode(P12, INPUT);
        digitalWrite(P12, HIGH); 
        pinMode(P13, INPUT);
        digitalWrite(P13, HIGH); 
        pinMode(P14, INPUT);
        digitalWrite(P14, HIGH); 
        pinMode(P15, INPUT);
        digitalWrite(P15, HIGH); 
        pinMode(P16, INPUT);
        digitalWrite(P16, HIGH); 
        digitalWrite(in_DHT11, HIGH); 
        pinMode(in_DHT11, INPUT);
        digitalWrite(in_DHT12, HIGH); 
        pinMode(in_DHT12, INPUT);
        
        digitalWrite(in_IR11, LOW); 
        pinMode(in_IR11, INPUT);
        
        digitalWrite(in_IR12, LOW); 
        pinMode(in_IR12, INPUT);
        
        digitalWrite(in_IR13, LOW); 
        pinMode(in_IR13, INPUT);
        
        
        
        Set_SimpleLight(L14);
        Set_SimpleLight(L16);
        Set_SimpleLight(L17);
        Set_SimpleLight(L19);
        
        Set_T52(DHT11_T);
        Set_T53(DHT11_U);
        Set_T52(DHT12_T);
        Set_T53(DHT12_U);
        Set_T42(ANTITHEFT);
        Set_T13(IR11);
        Set_T13(IR12);
        Set_T13(IR13);
}

void loop()
{ 
    // Here we start to play
    EXECUTEFAST() {                     
        UPDATEFAST();   
        
        FAST_110ms() {   // We process the logic and relevant input and output every 110 milliseconds

       
             
            if (!Alarm_Inserted &&(mOutput(IR11) == 1 || mOutput(IR12) == 1 || mOutput(IR13) == 1)){
              LowDigKeepHold(P13,Souliss_T1n_Timed+TempoCamere, Souliss_T1n_ToggleCmd, L16); 
              if (mInput (L16)>Souliss_T1n_Timed && mOutput(L16) == Souliss_T1n_OnCoil) mInput(L16) = Souliss_T1n_ToggleCmd;
            
              LowDigKeepHold(P12, Souliss_T1n_Timed+TempoScale,Souliss_T1n_ToggleCmd, L14);
              LowDigKeepHold(P14, Souliss_T1n_Timed+TempoScale,Souliss_T1n_ToggleCmd, L14);
              
              if (mOutput(IR12)==1 && !DAY)  mInput(L14) =Souliss_T1n_OnCmd;
              if (mOutput(IR12)==0 && !DAY)  mInput(L14) =Souliss_T1n_OffCmd;  
              if (mInput (L14)>Souliss_T1n_Timed && mOutput(L14) == Souliss_T1n_OnCoil) mInput(L14) = Souliss_T1n_ToggleCmd;
              if (mInput(L14) > Souliss_T1n_Timed)
                {
                  publish(LuceScalePT);
                }
             
              LowDigKeepHold(P15, Souliss_T1n_Timed+TempoSgabuzzino,Souliss_T1n_ToggleCmd, L19); 
               if (mInput (L19) > Souliss_T1n_Timed && mOutput(L19) == Souliss_T1n_OnCoil) mInput(L19) = Souliss_T1n_OffCmd;
              
              
              if (!DAY && mOutput(IR12) == 1) 
                mInput(L14) = Souliss_T1n_Timed+TempoScale; //Accendo la  luce Anticamera se e notte e rilevo movimento
             
             
            LowDigKeepHold(P16, Souliss_T1n_Timed+TempoCamere, Souliss_T1n_ToggleCmd, L17);  
              //if (mOutput(IR13)==1)  mInput(L17) =Souliss_T1n_OnCmd;
              //if (mOutput(IR13)==0)  mInput(L17) =Souliss_T1n_OffCmd;
              if (mInput (L17) > Souliss_T1n_Timed && mOutput(L17) == Souliss_T1n_OnCoil) mInput(L17) = Souliss_T1n_OffCmd;
              
            if (mInput(L17) == Souliss_T1n_ToggleCmd)
              {
                publish(SpegniLuci);
                mInput(L14)=Souliss_T1n_OffCmd;
                mInput(L16)=Souliss_T1n_OffCmd;
                mInput(L19)=Souliss_T1n_OffCmd;
                mInput(L17)=Souliss_T1n_OffCmd;
              }
            }
            
            if(subscribe_s(LuceScalePT))
              {
                mInput(L14) = Souliss_T1n_Timed+15;
              }
            if(subscribe_s(SpegniLuci))
              {
                mInput(L14)=Souliss_T1n_OffCmd;
                mInput(L16)=Souliss_T1n_OffCmd;
                mInput(L19)=Souliss_T1n_OffCmd;
                mInput(L17)=Souliss_T1n_OffCmd;
              }
            if(subscribe_s(LuceScalePT))
            {
              mInput(L14)=Souliss_T1n_Timed+TempoScale;
            }
            if(subscribe_s(Alarm_Armed)) Alarm_Inserted = true;
            if(subscribe_s(Alarm_Disarmed)) Alarm_Inserted = false;
            
            IR_AN = analogRead(in_IR11);
            if (IR_AN> IRONOFF) mInput(IR11) = Souliss_T1n_OnCmd;
            else  mInput(IR11) = Souliss_T1n_OffCmd;

            IR_AN = analogRead(in_IR12);
            if (IR_AN> IRONOFF) mInput(IR12) = Souliss_T1n_OnCmd;
            else  mInput(IR12) = Souliss_T1n_OffCmd;

            IR_AN = analogRead(in_IR13);
            if (IR_AN> IRONOFF) mInput(IR13) = Souliss_T1n_OnCmd;
            else  mInput(IR13) = Souliss_T1n_OffCmd;

            Logic_T13(IR11);
            Logic_T13(IR12);
            Logic_T13(IR13);

            
            
            if (test){
                
              
                if (mOutput(IR11) == 1)  mInput(L16) =Souliss_T1n_OnCmd;
                if (mOutput(IR11) == 0)  mInput(L16) =Souliss_T1n_OffCmd;
                if (mOutput(IR12) == 1)  mInput(L14) =Souliss_T1n_OnCmd;
                if (mOutput(IR12) == 0)  mInput(L14) =Souliss_T1n_OffCmd;
                if (mOutput(IR13) == 1)  mInput(L17) =Souliss_T1n_OnCmd;
                if (mOutput(IR13) == 0)  mInput(L17) =Souliss_T1n_OffCmd;
              }

              
            Logic_SimpleLight(L14);
            Logic_SimpleLight(L16);
            Logic_SimpleLight(L17);
            Logic_SimpleLight(L19);

            LowDigOut(O_L14, Souliss_T1n_Coil, L14); 
            LowDigOut(O_L16, Souliss_T1n_Coil, L16); 
            LowDigOut(O_L17, Souliss_T1n_Coil, L17); 
            LowDigOut(O_L19, Souliss_T1n_Coil, L19); 
    
            
        FAST_1110ms()   {
            
            
            if (Alarm_Inserted && (mOutput(IR11) || mOutput(IR12) || mOutput(IR13))) mInput(ANTITHEFT) = Souliss_T4n_Alarm; //Se attivo processo gli IR per allarme
            Logic_T42(ANTITHEFT,gateway_address);    
        }   
        }
        FAST_2110ms() { 
            mInput(ANTITHEFT) = Watchdog(gateway_address, WATCHDOG, Souliss_T4n_Alarm);
            
            
            

            if(memory_map[MaCaco_AUXIN_s + L16] > Souliss_T1n_Timed && mOutput(IR11) == 1)        // se accesa e rileva movimento
              memory_map[MaCaco_AUXIN_s + L16] = Souliss_T1n_Timed + TempoCamere; 
            if(memory_map[MaCaco_AUXIN_s + L17] > Souliss_T1n_Timed && mOutput(IR13) == 1)        // se accesa e rileva movimento
               memory_map[MaCaco_AUXIN_s + L17] = Souliss_T1n_Timed + TempoCamere; 
            if(memory_map[MaCaco_AUXIN_s + L14] > Souliss_T1n_Timed && mOutput(IR12) == 1)        // se accesa e rileva movimento
               memory_map[MaCaco_AUXIN_s + L14] = Souliss_T1n_Timed + TempoScale; 
          
            Timer_SimpleLight(L14);   //Anticamera
            
            if(subscribe_s(Giorno))
              {
                DAY = true;
              }
              if(subscribe_s(Notte))
              {
                DAY = false;
              }
          } 
        FAST_PeerComms(); 
    }    
        // Here we handle here the communication with Android, commands and notification
        // are automatically assigned to MYLEDLOGIC
                                      
        EXECUTESLOW() {
          UPDATESLOW();
              SLOW_10s() {  
               
                  Timer_SimpleLight(L16);   //Cameretta
                  Timer_SimpleLight(L17);   //Camera
                  Timer_SimpleLight(L19);   //Sgabuzzino
                  
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
                  ImportAnalog(DHT11_T, &t);
                  ImportAnalog(DHT11_U, &h);
  
                  
                  h = dht2.readHumidity();
                  t = dht2.readTemperature();
                  // Check if any reads failed and exit early (to try again).
                  if (isnan(h) || isnan(t) ) {
                      h = -999;
                      t = -999;
                  }
                  ImportAnalog(DHT12_T, &t);
                  ImportAnalog(DHT12_U, &h);
  
                  
                   Logic_T52(DHT11_T);
                   Logic_T53(DHT11_U);
                    
                   Logic_T52(DHT12_T);
                   Logic_T53(DHT12_U);
    
              } 
      }  
    
} 
