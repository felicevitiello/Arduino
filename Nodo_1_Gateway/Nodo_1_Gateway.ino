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
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/Webhook.h"                   // Enable DHCP and DNS

#include "felice_lib/casadomotica.h"        //File con indirizzi, publish subrcibe variabili comuni

   
  
// Include framework code and libraries
#include <SPI.h>
#include "Souliss.h"
// definisco gli slot associati alle logiche
//Nodo 1

//definisco gli slot
#define ANTITHEFT               0           // This is the memory slot used for the execution of the anti-theft
#define WATCHDOG                1           // This is the memory slot used for the execution of the watchdog
#define L1    2     // luce ingresso & scale
#define L2    3     // luce specchio bagno   
#define L4    4     // luce bagno pt
#define L5    5     // luce esterna porta ingresso
#define L6    6     // luce faretti ext
#define IR1   7     //IR Bagno  
#define IR2   8     //IR Scale
#define IR3   9     //IR Ingresso
#define DHT1_T 10
#define DHT1_U 12
#define Appoggio 14


//definisco gli Input
#define P1    2  
#define P4    1 
#define P6    6
#define P3    7
#define P2    8
#define P5    5 //Prima A5
#define in_DHT1 3
#define in_IR1  A6  // Prima 0  //Infrarosso Bagno
#define in_IR2  A7  // prima 4  //Infrarosso Scale
#define in_IR3  A5  // Prima 5  //Infrarosso Ingresso

// Definizione sensore DHT
#include "DHT.h"
#define DHTTYPE    DHT11  // DHT22 
DHT dht(in_DHT1, DHTTYPE, 15);

//Definisco gli output
#define O_L1  A0
#define O_L2  A1
#define O_L4  A2
#define O_L5  A3
#define O_L6  A4

// This sketch will use DHCP, but a generic IP address is defined in case
// the DHCP will fail. Generally this IP address will not be used and doesn't
// need to be used in SoulissApp

IPAddress ip(192, 168, 177, myvNet_address);
IPAddress gateway(192, 168, 177, ip_gateway_address);
IPAddress subnet(255, 255, 255, 0);


void setup()
{   
    Initialize();
  
    // Network configuration
    Ethernet.begin(ip, gateway, subnet);    // If DHCP fails, use a manual address                      
    SetAsGateway(gateway_address);       // Set this node as gateway for SoulissApp  
    SetAsPeerNode(Peer_address_2, 1); 
    SetAsPeerNode(Peer_address_3, 2); 
    SetAsPeerNode(Peer_address_4, 3); 

    //Serial.begin(9600);
    // We connect a pushbutton between 5V and pin2 with a pulldown resistor 
    // between pin2 and GND, the LED is connected to pin9 with a resistor to
    // limit the current amount
    
    //Definisco i PIN d'ingresso pulsanti, HW pulldown resistor inferiore a 50k



        digitalWrite(O_L1, HIGH);
        digitalWrite(O_L2, HIGH);
        digitalWrite(O_L4, HIGH);
        digitalWrite(O_L5, HIGH);
        digitalWrite(O_L6, HIGH);

        
        pinMode(P1, INPUT);
        digitalWrite(P1, HIGH);           //internal pullup resistor                  
        pinMode(P2, INPUT);
        digitalWrite(P2, HIGH);           //internal pullup resistor
        pinMode(P3, INPUT);
        digitalWrite(P3, HIGH);           //internal pullup resistor
        pinMode(P4, INPUT);
        digitalWrite(P4, HIGH);           //internal pullup resistor
        pinMode(P5, INPUT);
        digitalWrite(P5, HIGH);           //internal pullup resistor
        pinMode(P6, INPUT);
        digitalWrite(P6, HIGH);           //internal pullup resistor
       
        pinMode(in_DHT1, INPUT);
        digitalWrite(in_DHT1, HIGH);                  
        
        pinMode(in_IR1, INPUT);
        pinMode(in_IR2, INPUT);
        pinMode(in_IR3, INPUT);


        
        pinMode(O_L1, OUTPUT);                
        pinMode(O_L2, OUTPUT);
        pinMode(O_L4, OUTPUT);
        pinMode(O_L5, OUTPUT);
        pinMode(O_L6, OUTPUT);    


        Set_SimpleLight(L1);
        Set_SimpleLight(L2);
        Set_SimpleLight(L4);
        Set_SimpleLight(L5);
        Set_SimpleLight(L6);
        Set_T52(DHT1_T);
        Set_T53(DHT1_U);
        Set_T41(ANTITHEFT);
        Set_T13(IR1);
        Set_T13(IR2);
        Set_T13(IR3);
      
  //definisco le variabili funzionali
      
}

void loop()
{ 
    EXECUTEFAST() {                     
          UPDATEFAST();   
          //FAST_110ms()
          SHIFT_110ms(0){   // We process the logic and relevant input and output every 50 milliseconds
              bool test=false;
              if (!Alarm_Inserted && !test){
                if (mOutput(IR1)=1) LowDigKeepHold(P6, Souliss_T1n_Timed+TempoBagnoSpecchio, Souliss_T1n_ToggleCmd, L2); //Luce Bagno Specchio
                if (mInput (L2)>Souliss_T1n_Timed && mOutput(L2) == Souliss_T1n_OnCoil) mInput(L2) = Souliss_T1n_OffCmd;
                
                LowDigKeepHold(P4, Souliss_T1n_Timed+TempoBagno, Souliss_T1n_ToggleCmd, L4);//Luce Bagno alta
                if (mInput (L4)>Souliss_T1n_Timed && mOutput(L4) == Souliss_T1n_OnCoil) mInput(L4) = Souliss_T1n_OffCmd;

                LowDigKeepHold(P3, Souliss_T1n_Timed+TempoLuciExt, Souliss_T1n_ToggleCmd, L5); //Luce Esterna porta Alta
                if (mInput (L5)>Souliss_T1n_Timed && mOutput(L5) == Souliss_T1n_OnCoil) mInput(L5) = Souliss_T1n_OffCmd;
                
                LowDigKeepHold(P1, Souliss_T1n_Timed+TempoScale,Souliss_T1n_ToggleCmd, L1); //Luce ingresso e Scale
                
                if (mInput(L1) == Souliss_T1n_Timed+TempoScale)
                {
                  publish(LuceScalePT);
                }
                if (!DAY && (mOutput(IR2) == 1 && mOutput(IR3))) 
                mInput(L1) = Souliss_T1n_Timed+TempoScale; //Accendo la  luce Anticamera se e notte e rilevo movimento     
              }
              
              if (test){
                
              
                if (mOutput(IR1)==1)  mInput(L2) =Souliss_T1n_OnCmd;
                if (mOutput(IR1)==0)  mInput(L2) =Souliss_T1n_OffCmd;
                if (mOutput(IR2)==1)  mInput(L1) =Souliss_T1n_OnCmd;
                if (mOutput(IR2)==0)  mInput(L1) =Souliss_T1n_OffCmd;
                if (mOutput(IR3)==1)  mInput(L5) =Souliss_T1n_OnCmd;
                if (mOutput(IR3)==0)  mInput(L5) =Souliss_T1n_OffCmd;
              }
          }
              
              
              
          SHIFT_110ms(1){    
             LowDigKeepHold(P5, Souliss_T1n_OnCmd, Souliss_T1n_ToggleCmd, Appoggio); //Pulsante campanello ext
             if (mInput(Appoggio) == Souliss_T1n_OnCmd)
             {
              publish(Campanello);
              mInput(Appoggio) =0x0;
             }
             else if (mInput(Appoggio) == Souliss_T1n_ToggleCmd)
             {
              mInput(L5)=Souliss_T1n_Timed+TempoLuciExt;
              mInput(L6)=Souliss_T1n_Timed+TempoLuciExt;
              mInput(Appoggio) =0x0;
             }
  

              
              LowDigInHold(P2, Souliss_T1n_ToggleCmd,Souliss_T1n_ToggleCmd+5, L6);            // Spegne tutte le luci o Luce Faretti Esterni
              if (mInput(L6) == Souliss_T1n_ToggleCmd+5) 
              {
                mInput(L6) = 0;
                publish(SpegniLuci);
                mInput(L1) = Souliss_T1n_OffCmd;
                mInput(L2) = Souliss_T1n_OffCmd;
                mInput(L4) = Souliss_T1n_OffCmd;
                if (!DAY)
                {
                  mInput(L5) = Souliss_T1n_Timed+TempoLuciExt;
                  mInput(L6) = Souliss_T1n_Timed+TempoLuciExt;
                } else {
                  mInput(L5) = Souliss_T1n_OffCmd;
                  mInput(L6) = Souliss_T1n_OffCmd;
                }
               
              }
             }

             
              SHIFT_110ms(2){
              if(subscribe(SpegniLuci))
              {
               mInput(L1) = Souliss_T1n_OffCmd;
               mInput(L2) = Souliss_T1n_OffCmd;
               mInput(L4) = Souliss_T1n_OffCmd;
               mInput(L5) = Souliss_T1n_OffCmd;
               mInput(L6) = Souliss_T1n_OffCmd;
              }
              if(subscribe(LuceScalePT)){
               mInput(L1) = Souliss_T1n_Timed+TempoScale;
              }
              
              
              Logic_SimpleLight(L1);
              Logic_SimpleLight(L2);
              Logic_SimpleLight(L4);
              Logic_SimpleLight(L5);
              Logic_SimpleLight(L6);
            
              LowDigOut(O_L1, Souliss_T1n_Coil, L1); 
              LowDigOut(O_L2, Souliss_T1n_Coil, L2); 
              LowDigOut(O_L4, Souliss_T1n_Coil, L4); 
              LowDigOut(O_L5, Souliss_T1n_Coil, L5); 
              LowDigOut(O_L6, Souliss_T1n_Coil, L6);    
          } 
          FAST_1110ms()   {
              IR_AN = analogRead(in_IR1);
              if (IR_AN> IRONOFF) mInput(IR1) = Souliss_T1n_OnCmd;
              else  mInput(IR1) = Souliss_T1n_OffCmd;

              IR_AN = analogRead(in_IR2);
              if (IR_AN> IRONOFF) mInput(IR2) = Souliss_T1n_OnCmd;
              else  mInput(IR2) = Souliss_T1n_OffCmd;

              IR_AN = analogRead(in_IR3);
              if (IR_AN> IRONOFF) mInput(IR3) = Souliss_T1n_OnCmd;
              else  mInput(IR3) = Souliss_T1n_OffCmd;
                         
              Logic_T13(IR1);
              Logic_T13(IR2);
              Logic_T13(IR3);
          }
                   
          FAST_2110ms()   {   // Execute the code every 2110ms    
              if(mAuxiliary(L1)> Souliss_T1n_Timed && mOutput(IR2) == 1)        // se accesa e rileva movimento
              mAuxiliary(L1) = Souliss_T1n_Timed + TempoScale;  
              
              if(mAuxiliary(L2) > Souliss_T1n_Timed && mOutput(IR1) == 1)        // se accesa e rileva movimento
              mAuxiliary(L2) = Souliss_T1n_Timed + TempoBagnoSpecchio;  

              if(mAuxiliary(L4) > Souliss_T1n_Timed && mOutput(IR1) == 1)        // se accesa e rileva movimento
              mAuxiliary(L4) = Souliss_T1n_Timed + TempoBagno;  

              
              Timer_SimpleLight(L1);
              Timer_SimpleLight(L5);
              Timer_SimpleLight(L6);
              
              
              
              

              
              if(subscribe(Day))
              {
                DAY = true;
              }
              if(subscribe(Night))
              {
                DAY = false;
              }
             if (Alarm_Inserted) { 
                publish(Alarm_Armed);
                if ( Timer_Alarm == 0 && (mOutput(IR1) == 1 || mOutput(IR2) == 1 || mOutput(IR3) == 1)) 
                  mInput(ANTITHEFT) = Souliss_T4n_Alarm;
                if (Timer_Alarm == 0 && subscribe(Alarm_Alarm))  mInput(ANTITHEFT) = Souliss_T4n_Alarm;
             }
             // Build a watchdog chain to monitor the nodes
             mInput(ANTITHEFT) = Watchdog(Peer_address_2, WATCHDOG, Souliss_T4n_Alarm);
             Logic_T41(ANTITHEFT);
          }  
                  
          // Here we handle here the communication with Android, commands and notification
          // are automatically assigned to MYLEDLOGIC
          FAST_GatewayComms();                                           
      }
      EXECUTESLOW() {
          UPDATESLOW();
              SLOW_10s() {  
                  if (Alarm_Inserted) {
                    if (Timer_Alarm != 0)  Timer_Alarm--;
                   }
                  else Timer_Alarm = T_Alarm;

                  
                  Timer_SimpleLight(L2);
                  Timer_SimpleLight(L4);
                  if (mOutput(ANTITHEFT) == Souliss_T4n_Alarm) {
                    // Inserire l'azione da compiere, invio a pushetta, SMS, oppure altre azioni
                    //Serial.println("Allarme!");
                  }
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
                  ImportAnalog(DHT1_T, &t);
                  ImportAnalog(DHT1_U, &h);
  
                   Logic_T52(DHT1_T);
                   Logic_T53(DHT1_U);
    
              } 
      }     
} 
