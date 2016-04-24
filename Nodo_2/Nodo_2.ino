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
//Nodo 2

//definisco gli slot
#define ANTITHEFT               0           // This is the memory slot used for the execution of the anti-theft
#define WATCHDOG                1           // This is the memory slot used for the execution of the watchdog
#define L6    2                             //CUCINA
#define L7    3 
#define L8    4
#define L9    5                             //SOGGIORNO
#define O10   6 //CAMPANELLO
#define IR4   7 //Cucina
#define IR5   8 //Soggiorno
#define DHT2_T  9  
#define DHT2_U  11

//definisco gli Input

#define P7    A1 //Cucina 
#define P8    A2  //Esterno
#define P9    A3  //Soggiorno
#define in_IR4   A0//IR Cucina
#define in_IR5   A4
#define in_DHT2  A5

#include "DHT.h"
#define DHTTYPE    DHT11  // DHT22 
DHT dht(in_DHT2, DHTTYPE, 15);


//Definisco gli output
#define O_L6  2  //Luce Cucina
#define O_L7  3  //Luce soggiorno 1
#define O_L8  4  //Luce esterna
#define O_L9  5  //Luce soggiorno 2
#define O_OC10 6 //USCITA CAMPANELLO




// This sketch will use DHCP, but a generic IP address is defined in case
// the DHCP will fail. Generally this IP address will not be used and doesn't
// need to be used in SoulissApp

IPAddress ip(192, 168, 177, Peer_address_2);
IPAddress gateway(192, 168, 177, ip_gateway_address);
IPAddress subnet(255, 255, 255, 0);


void setup()
{   
    Initialize();

    // Network configuration
    //if(Ethernet.begin()==0)                     // Get an address via DHCP     
        Ethernet.begin(ip, gateway, subnet);    // If DHCP fails, use a manual address
        //SetAddress(0xAB02, 0xFF00, 0xAB01);
    //else
    //    ip = Ethernet.localIP();                            
    //SetAsGateway(gateway_address);       // Set this node as gateway for SoulissApp  
    
    // We connect a pushbutton between 5V and pin2 with a pulldown resistor 
    // between pin2 and GND, the LED is connected to pin9 with a resistor to
    // limit the current amount
    
    //Definisco i PIN d'ingresso pulsanti, HW pulldown resistor inferiore a 50k




        digitalWrite(O_L6, HIGH);
        digitalWrite(O_L7, HIGH);
        digitalWrite(O_L8, HIGH);
        digitalWrite(O_L9, HIGH);
        digitalWrite(O_OC10, HIGH);
        
        pinMode(P7, INPUT);                  // Hardware pulldown required
        digitalWrite(P7, HIGH);           //internal pullup resistor
        pinMode(P8, INPUT);                  // Hardware pulldown required
        digitalWrite(P8, HIGH);            //internal pullup resistor        
        pinMode(P9, INPUT);                  // Hardware pulldown required
        digitalWrite(P9, HIGH);           //internal pullup resistor


        digitalWrite(in_DHT2, LOW);
        digitalWrite(in_DHT2, LOW); 
        pinMode(in_IR4, INPUT);
        pinMode(in_IR5, INPUT);
        
        digitalWrite(in_DHT2, HIGH);
        pinMode(in_DHT2, INPUT);
        
        pinMode(O_L6, OUTPUT);                
        pinMode(O_L7, OUTPUT);
        pinMode(O_L8, OUTPUT);
        pinMode(O_L9, OUTPUT);
        pinMode(O_OC10, OUTPUT);
      

        Set_SimpleLight(L6);
        Set_SimpleLight(L7);
        Set_SimpleLight(L8);
        Set_SimpleLight(L9);
        Set_PulseOutput(O10);
        Set_T52(DHT2_T);
        Set_T53(DHT2_U);
        Set_T42(ANTITHEFT);
        Set_T13(IR4);
        Set_T13(IR5);
      
}



void loop()
{ 

    

      // Here we start to play
      EXECUTEFAST() {                     
          UPDATEFAST();   
          
          FAST_110ms() {   // We process the logic and relevant input and output every 110 milliseconds

              if (!Alarm_Inserted   && (mOutput(IR4) || mOutput(IR5))){
                LowDigKeepHold(P9,Souliss_T1n_Timed+TempoSoggiorno, Souliss_T1n_ToggleCmd, L7); //Luce Soggiorno
                if (mInput (L7)>Souliss_T1n_Timed && mOutput(L7) == Souliss_T1n_OnCoil) mInput(L7) = Souliss_T1n_ToggleCmd;
                
                LowDigKeepHold(P8, Souliss_T1n_Timed+TempoLuciExt,Souliss_T1n_ToggleCmd, L8);
                if (mInput (L8)>Souliss_T1n_Timed && mOutput(L8) == Souliss_T1n_OnCoil) mInput(L8) = Souliss_T1n_ToggleCmd;
                
                
                LowDigKeepHold(P7, Souliss_T1n_Timed+TempoCucina, Souliss_T1n_ToggleCmd, L6);  //Luce Cucina
                if (mInput (L6)>Souliss_T1n_Timed && mOutput(L6) == Souliss_T1n_OnCoil) mInput(L6) = Souliss_T1n_ToggleCmd;
                
                mInput(L9) = mInput(L7);
             }
              if(subscribe_s(Campanello))
              {
                mInput(O10)=Souliss_T1n_OnCmd;
              }
              if(subscribe_s(SpegniLuci))
              {
                mInput(L6)=Souliss_T1n_OffCmd;
                mInput(L7)=Souliss_T1n_OffCmd;
                mInput(L8)=Souliss_T1n_OffCmd;
                mInput(L9)=Souliss_T1n_OffCmd;
                if (!DAY)
                {
                  mInput(L8) = Souliss_T1n_Timed+TempoLuciExt;
                } else {
                  mInput(L8) = Souliss_T1n_OffCmd;
                }
              }
              if(subscribe_s(Alarm_Armed)) Alarm_Inserted = true;
              if(subscribe_s(Alarm_Disarmed)) Alarm_Inserted = false;
              
              IR_AN = analogRead(in_IR4);
              if (IR_AN> IRONOFF) mInput(IR4) = Souliss_T1n_OnCmd;
              else  mInput(IR4) = Souliss_T1n_OffCmd;

              IR_AN = analogRead(in_IR5);
              if (IR_AN> IRONOFF) mInput(IR5) = Souliss_T1n_OnCmd;
              else  mInput(IR5) = Souliss_T1n_OffCmd;
            
              Logic_T13(IR4);
              Logic_T13(IR5); 
              
              if (test){
                if (mOutput(IR4)==1)  mInput(L7) =Souliss_T1n_OnCmd;
                if (mOutput(IR4)==0)  mInput(L7) =Souliss_T1n_OffCmd;
                if (mOutput(IR5)==1)  mInput(L6) =Souliss_T1n_OnCmd;
                if (mOutput(IR5)==0)  mInput(L6) =Souliss_T1n_OffCmd;
                 mInput(L9) = mInput(L7);
              }
             

              Logic_SimpleLight(L6);
              Logic_SimpleLight(L7);
              Logic_SimpleLight(L8);
              Logic_SimpleLight(L9);
              
             
              LowDigOut(O_L6, Souliss_T1n_Coil, L6); 
              LowDigOut(O_L7, Souliss_T1n_Coil, L7);
              LowDigOut(O_L9, Souliss_T1n_Coil, L7); 
              LowDigOut(O_L8, Souliss_T1n_Coil, L8); 
            
              
          } 
          FAST_1110ms()   {
             

              if (Alarm_Inserted && (mOutput(IR4) || mOutput(IR5))) mInput(ANTITHEFT) = Souliss_T4n_Alarm; //Se attivo processo gli IR per allarme
              Logic_T42(ANTITHEFT,gateway_address);
          }
          FAST_2110ms() {
            if(memory_map[MaCaco_AUXIN_s + L7] > Souliss_T1n_Timed && memory_map[MaCaco_OUT_s + IR5] == Souliss_T1n_OnCoil)        // se accesa e rileva movimento
              memory_map[MaCaco_AUXIN_s + L7] = Souliss_T1n_Timed + TempoSoggiorno;  
              
            if(memory_map[MaCaco_AUXIN_s + L6] > Souliss_T1n_Timed && memory_map[MaCaco_OUT_s + IR4] == Souliss_T1n_OnCoil)        // se accesa e rileva movimento
            memory_map[MaCaco_AUXIN_s + L6] = Souliss_T1n_Timed + TempoCucina;  
            
            
            Timer_SimpleLight(L8);
             if(subscribe_s(Giorno))
              {
                DAY = true;
              }
              if(subscribe_s(Notte))
              {
                DAY = false;
              }
            
            Logic_PulseOutput(O10);
            LowDigOut(O_OC10, Souliss_T1n_Coil, O10); 
            mInput(ANTITHEFT) = Watchdog(Peer_address_3, WATCHDOG, Souliss_T4n_Alarm);
          }
          FAST_PeerComms(); 
      }
                                            
          EXECUTESLOW() {
              UPDATESLOW();
              SLOW_10s() {  

                  Timer_SimpleLight(L6);
                  Timer_SimpleLight(L7);
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
                  ImportAnalog(DHT2_T, &t);
                  ImportAnalog(DHT2_U, &h);
                  
                   Logic_T52(DHT2_T);
                   Logic_T53(DHT2_U);
    
              } 
      }
     
    
} 
