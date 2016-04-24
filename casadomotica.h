//Define Costanti di sistema Tutti i tempi vanno moltiplicati per x2,1  la fast 2100ms in cui sono inseriti
#define TempoLuciExt	45
#define TempoScale		5
//Define Costanti di sistema Tutti i tempi vanno moltiplicati per x10 la slo 10s in cui sono inseriti
#define	TempoCucina		120
#define	TempoBagno		30
#define	TempoBagnoSpecchio	30
#define	TempoCamere		60
#define	TempoSoggiorno	60
#define	TempoSgabuzzino	60
#define T_Alarm			6
#define	TempoSensoriOff_Max	30

//Define per Publish/Subscribe

#define Campanello  0x0003,0x02
#define LuceScalePT 0x0003,0x03
#define LuceScalePP 0x0003,0x04
#define SpegniLuci  0x0003,0x05
#define Giorno      0x0004,0x04
#define Notte       0x0004,0x05
#define Alarm_Armed 0x0004,0x06
#define Alarm_Disarmed	0x0004,0x07
#define	Alarm_Alarm	0x0004,0x06

//Define per indirizzi di rete

#define ip_gateway_address 1
#define gateway_address 51
#define myvNet_address  51
#define Peer_address_2  52
#define Peer_address_3  53
#define Peer_address_4  54

//Varianbili Globali Inizializzate
bool PrimoAvvio = true;
bool DAY=true;
bool Alarm_Inserted=false;
int IR_AN;
int Timer_Alarm = T_Alarm;
int TempoSensoriOff = 0;
unsigned long t;
unsigned long currentMillis;
bool presenza_rete = true;
bool presenza_rete_precedente = true;
bool test=false;

//Costante per stabilire stato ON/OFF dei sensori infrarosso
#define IRONOFF	450
