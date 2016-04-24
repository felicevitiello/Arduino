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

// routine Tempo: http://playground.arduino.cc//Code/Webclient


// Configure the framework
#include "bconf/StandardArduino.h"          // Use a standard Arduino
#include "conf/ethW5100.h"                  // Ethernet through Wiznet W5100
#include "conf/Gateway.h"                   // The main node is the Gateway, we have just one node
#include "conf/Webhook.h"                   // Enable DHCP and DNS
#include "felice_lib/casadomotica.h"        //File con indirizzi, publish subrcibe variabili comuni
#include <TimeLib.h>
   
  
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
#define P1    2  //marrone
#define P4    4 //SOSTITUIRE CON 1 //nero
#define P6    6 //nero
#define P3    7 //rosa grigio
#define P2    8//marrone
#define P5    5 //Bianco
#define in_DHT1 3 //verde
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
uint8_t ip_address[4]  = {192, 168, 177, 51}; //indirizzo peer corrente
uint8_t subnet_mask[4] = {255, 255, 255, 0}; //subnet
uint8_t ip_gateway[4]  = {192, 168, 177, 1}; // indirizzo gateway internet
uint8_t gatewaysouliss[4]  = {192, 168, 177, 51}; // indirizzo gateway Souliss
#define myvNet_gateway  gatewaysouliss[3]

//PUSHETTA
char APIKEY[] = "ae86a8a0c7a9b511805284264a8b365924de53e9";   // Put here your API key
char CHANNEL[] = "RED CASA";                                  // and here your channel name
//char serverName[] = "api.pushetta.com";
char serverName[] = "107.0.22.123";
boolean lastConnected = false; 
EthernetClient client;


//Imposto variabili per TIME
    unsigned long unixTime;
    String DataOra;

//Function for sending the request to Pushetta
void sendToPushetta(char channel[], String text) {
  client.stop();
  Serial.println ("Invio a Pushetta..."); 
  if (client.connect(serverName, 80))
  {
    Serial.println ("Client Connesso..."); 
    client.print("POST /api/pushes/");
    client.print(channel);
    client.println("/ HTTP/1.1");
    client.print("Host: ");
    client.println(serverName);
    client.print("Authorization: Token ");
    client.println(APIKEY);
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(text.length() + 46);
    client.println();
    client.print("{ \"body\" : \"");
    client.print(text);
    client.println("\", \"message_type\" : \"text/plain\" }");
    client.println();
    Serial.println ("Messaggio Inviato!"); 
  }
}

unsigned long webUnixTime (Client &client)
{
  unsigned long time = 0;

  // Just choose any reasonably busy web server, the load is really low
  if (client.connect("www.gazzetta.it", 80))
    {
      // Make an HTTP 1.1 request which is missing a Host: header
      // compliant servers are required to answer with an error that includes
      // a Date: header.
      client.print(F("GET / HTTP/1.1 \r\n\r\n"));

      char buf[5];      // temporary buffer for characters
      client.setTimeout(5000);
      if (client.find((char *)"\r\nDate: ") // look for Date: header
    && client.readBytes(buf, 5) == 5) // discard
  {
    unsigned day = client.parseInt();    // day
    client.readBytes(buf, 1);    // discard
    client.readBytes(buf, 3);    // month
    int year = client.parseInt();    // year
    byte hour = client.parseInt();   // hour
    byte minute = client.parseInt(); // minute
    byte second = client.parseInt(); // second

    int daysInPrevMonths;
    switch (buf[0])
      {
      case 'F': daysInPrevMonths =  31; break; // Feb
      case 'S': daysInPrevMonths = 243; break; // Sep
      case 'O': daysInPrevMonths = 273; break; // Oct
      case 'N': daysInPrevMonths = 304; break; // Nov
      case 'D': daysInPrevMonths = 334; break; // Dec
      default:
        if (buf[0] == 'J' && buf[1] == 'a')
    daysInPrevMonths = 0;   // Jan
        else if (buf[0] == 'A' && buf[1] == 'p')
    daysInPrevMonths = 90;    // Apr
        else switch (buf[2])
         {
         case 'r': daysInPrevMonths =  59; break; // Mar
         case 'y': daysInPrevMonths = 120; break; // May
         case 'n': daysInPrevMonths = 151; break; // Jun
         case 'l': daysInPrevMonths = 181; break; // Jul
         default: // add a default label here to avoid compiler warning
         case 'g': daysInPrevMonths = 212; break; // Aug
         }
      }

    // This code will not work after February 2100
    // because it does not account for 2100 not being a leap year and because
    // we use the day variable as accumulator, which would overflow in 2149
    day += (year - 1970) * 365; // days from 1970 to the whole past year
    day += (year - 1969) >> 2;  // plus one day per leap year 
    day += daysInPrevMonths;  // plus days for previous months this year
    if (daysInPrevMonths >= 59  // if we are past February
        && ((year & 3) == 0)) // and this is a leap year
      day += 1;     // add one day
    // Remove today, add hours, minutes and seconds this month
    time = (((day-1ul) * 24 + hour) * 60 + minute) * 60 + second;
  }
    }
  delay(10);
  client.flush();
  client.stop();

  return time;
}


void setup()
{   
     Initialize();
     //Ethernet.begin(ip, gateway, subnet);
     Ethernet.begin();
     SetIPAddress(ip_address, subnet_mask, ip_gateway);
     SetAsGateway(gateway_address);       
     SetAsPeerNode(Peer_address_2, 1); 
     SetAsPeerNode(Peer_address_3, 2); 
     SetAsPeerNode(Peer_address_4, 3); 

    Serial.begin(9600);
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
      
  //dSincronizzo l'orologio
  unixTime = 0;//webUnixTime(client);
  setTime(unixTime);
  Serial.println(unixTime);   
}

void loop()
{ 
    EXECUTEFAST() {                     
          UPDATEFAST();   
          //FAST_110ms()
          SHIFT_110ms(0){   // We process the logic and relevant input and output every 50 milliseconds
              bool test=false;
              if (!Alarm_Inserted && !test){
                if (mOutput(IR1)) LowDigKeepHold(P6, Souliss_T1n_Timed+TempoBagnoSpecchio, Souliss_T1n_ToggleCmd, L2); //Luce Bagno Specchio
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

              //if (((mOutput(IR1)  || mOutput(IR2)  || mOutput(IR3) ) && (TempoSensoriOff == TempoSensoriOff_Max)) || PrimoAvvio)  {
              if (PrimoAvvio)  {
                unixTime= now();
                DataOra = String(year(unixTime))+"-"+String(month(unixTime))+"-"+String(day(unixTime))+" "+String(hour(unixTime))+":"+String(minute(unixTime))+":"+String(second(unixTime));  
                String thisString;
                if (!PrimoAvvio) {
                  thisString = DataOra +" :  : Movimento rilevato- IR BAGNO: "+ String(mOutput(IR1))+ ", IR INGR: "+String(mOutput(IR2))+", IR Scale: "+String(mOutput(IR3));
                  Serial.println (thisString);
                  sendToPushetta(CHANNEL,thisString ); // send to Pushetta
                  TempoSensoriOff = 0;
                } else {
                  String thisString = "Evento ESP Fotocellula:"+ DataOra;
                  Serial.println (thisString);
                  
                  sendToPushetta(CHANNEL,"Pippo"); // send to Pushetta
                  PrimoAvvio = false;
                }  
              }

              Logic_T41(ANTITHEFT);
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
              
              
              
              

              
              if(subscribe(Giorno))
              {
                DAY = true;
              }
              if(subscribe(Notte))
              {
                DAY = false;
              }
             if (Alarm_Inserted) { 
                publish(Alarm_Armed);
                if ( Timer_Alarm == 0 && (mOutput(IR1) == 1 || mOutput(IR2) == 1 || mOutput(IR3) == 1)) 
                  mInput(ANTITHEFT) = Souliss_T4n_Alarm;
             }
             // Build a watchdog chain to monitor the nodes
             mInput(ANTITHEFT) = Watchdog(Peer_address_2, WATCHDOG, Souliss_T4n_Alarm); 
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

                  if (TempoSensoriOff < TempoSensoriOff_Max)  TempoSensoriOff++; //incremento il tempo di conteggio da quando ho rilevato l'ultimo movimento.
                  
                  Timer_SimpleLight(L2);
                  Timer_SimpleLight(L4);
                  if (mOutput(ANTITHEFT) == Souliss_T4n_Alarm) {
                    sendToPushetta(CHANNEL, "Allarme da Casa di Via Malagotta!"); // send to Pushetta
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
                   sendToPushetta(CHANNEL,"Messaggio temporizzato 10s");
              } 
              SLOW_1day()  {
                unixTime = webUnixTime(client);
                if (unixTime != 0)
                setTime(unixTime);
              }
      }     
} 



