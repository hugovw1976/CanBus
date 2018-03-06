#include <Metro.h>
#include <FlexCAN.h>
#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <EEPROM.h>
#include "ClickButton.h"
#define RED 0x0
#define BLUE 0x1
#include "printf.h"

Metro cansend = Metro(20);// milliseconds

RF24 radio(9,10);
// Pins on the remote for buttons
const uint8_t num_button_pins = 10;
static CAN_message_t msg;
//static CAN_message_t msg0;
static CAN_message_t msg1;

struct payload_t
{
  uint16_t buttons;
};

uint8_t button_states[10];

const int buttons = 10;
int function[buttons] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

unsigned long waitUntil = 0;

int beep1_counter=0;
int beep2_counter=0;
int beep3_counter=0;
int beep4_counter=0;
int beep5_counter=0;
int beep6_counter=0;
int beep7_counter=0;
int beep8_counter=0;
int beep9_counter=0;
int beep10_counter=0;
int Hazard = 0;
int Strobe = 0;
int Rain = 0;
int Lturn = 0;
int Rturn = 0;
int DLR = EEPROM.read(0);
int StrobeFunction = EEPROM.read(1);
int Reverse = 0;
int Lights = 0;
int HighBeam = 0;
int Speed = 0;
int Windshield = 0;
int Horn = 0;
int Washer = 0;
int count = 0;
int wiper_funtion = 0;
int wiper_funtion1 = 0;
int wiper_funtion2 = 0;
int Lights_Radio=0;

const int buttonPin0=24;
const int buttonPin1=25;
const int buttonPin2=26;
const int buttonPin3=27;
const int buttonPin4=28;
const int buttonPin5=29;
const int buttonPin6=30;
const int buttonPin7=31;
const int buttonPin8=32;
const int buttonPin9=33;

unsigned char bit0;
unsigned char bit1;
unsigned char bit2;
unsigned char bit3;
unsigned char bit4;
unsigned char bit5;
unsigned char bit6;
unsigned char bit7;

ClickButton button[10] = {
ClickButton (buttonPin0, HIGH),
ClickButton (buttonPin1, HIGH),
ClickButton (buttonPin2, HIGH),
ClickButton (buttonPin3, HIGH),
ClickButton (buttonPin4, HIGH),
ClickButton (buttonPin5, HIGH),
ClickButton (buttonPin6, LOW),
ClickButton (buttonPin7, HIGH),
ClickButton (buttonPin8, HIGH),
ClickButton (buttonPin9, HIGH),
}; 
// -------------------------------------------------------------
void setup(){

  Can0.begin(100000);
  radio.begin();
  Serial.begin(9600);
  radio.setChannel(10);//0:steering.V1, 5:test_board.v1, 10:steering.V2
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(10,10);
  radio.openReadingPipe(1,0xE7E7E7E7E7LL);
  radio.startListening();

  for (int i=0; i<buttons; i++)
  { 
    // Setup button timers (all in milliseconds / ms)
    // (These are default if not set, but changeable for convenience)
    button[i].debounceTime   = 20;   // Debounce timer in ms
    button[i].multiclickTime = 250;  // Time limit for multi clicks
    button[i].longClickTime  = 1000; // Time until long clicks register
  }    
}
// -------------------------------------------------------------

void loop(){
    while (radio.available())
      {
        if ( radio.available() ) {
        radio.read( button_states, num_button_pins );
        //printf("Got buttons\n\r");
        }
      }
  
// -------------------------------------------------------------
    if (cansend.check() == 1) {
    msg.len = 8;
    msg.id = 150;
    msg.buf[0] = bit0;
    msg.buf[1] = bit1;
    msg.buf[2] = bit2;
    msg.buf[3] = bit3;
    msg.buf[4] = bit4;
    msg.buf[5] = bit5;
    msg.buf[6] = bit6;
    msg.buf[7] = bit7;
    Can0.write(msg); 

    msg1.len = 8;
    msg1.id = 1589;
    msg1.buf[0] = Lights_Radio;
    msg1.buf[1] = 0;
    msg1.buf[2] = 0;
    msg1.buf[3] = 0;
    msg1.buf[4] = 0;
    msg1.buf[5] = 0;
    msg1.buf[6] = 0;
    msg1.buf[7] = 0;
    Can0.write(msg1);
    }
    
// -------------------------------------------------------------
  for (int i=0; i<10; i++)
  {
  button[i].buttonState = button_states[i];
  button[i].Update();
  if (button[i].clicks != 0) function[i] = button[i].clicks;
  }
// -------------------------------------------------------------L-Turn

  if(button[8].clicks == 1) Lturn = !Lturn;
  if(Lturn == 1){
    Rturn = 0; 
    lturn();
  }
// -------------------------------------------------------------R-Turn

  if(button[7].clicks == 1) Rturn = !Rturn;
  if(Rturn == 1){
    Lturn =0;
    rturn();
  }
// -------------------------------------------------------------Hazard
  int rain_status,strobe_status;
  if(button[2].clicks == 1) Hazard = !Hazard;
  if(Hazard == 1){
    Lturn = 0;
    Rturn = 0;
    hazard();
  }
  if((Lturn == 0) && (Rturn == 0) && (Hazard == 0)) {
    beep1_counter = 0;
    beep2_counter = 0;
    bit0=0;
    //noTone(6); 
  }
  if(button[2].clicks == 2) Rain = !Rain;
  if(Rain == 1) rain_status=1;
  if(Rain == 0) rain_status=0;
  if(button[2].clicks == -1) Strobe = !Strobe;
  if(Strobe == 0){
    strobe_status=0;
  }
  if(Strobe == 1){
    Lturn = 0;
    Rturn = 0;
    Hazard = 0;
    Lights = 0;
    bit1=0;
    bit2=0;
    strobe_status=2;
  }
  if(strobe_status == 2){
    beep3_counter = beep3_counter+1;
    beep4_counter = 0;
    if(beep3_counter < 15000)tone(6, 3000);
    if(beep3_counter == 15000)noTone(6);
  }
  if(strobe_status == 0){
    beep4_counter = beep4_counter+1;
    beep3_counter = 0;
    if(beep4_counter < 15000)tone(6, 3000);
    if(beep4_counter == 15000)noTone(6);
  }
  bit3=rain_status+strobe_status;

//--------------------------------------------------------------DLR

       if ((Lturn==0)&&(Hazard==0)&&(Lights==0)&&(Reverse==0)&&(DLR==1)&&(Strobe==0)){bit2=1;}
       else{if ((Lturn!=0)||(Hazard!=0)||(DLR==0)){if((Reverse==0)&&(Strobe==0)){bit2=0;}}}
         
       if ((Rturn==0)&&(Hazard==0)&&(Lights==0)&&(Reverse==0)&&(DLR==1)&&(Strobe==0)){bit1=1;}
       else{if ((Rturn!=0)||(Hazard!=0)||(DLR==0)){if((Reverse==0)&&(Strobe==0)){bit1=0;}}} 
        
// -------------------------------------------------------------Lights
int bit4state;
if(button[1].clicks == 1) Lights = Lights+1;
if(Lights==1){
  Lights_Radio=1;
  if((Lturn==0)&&(Hazard==0)){DLR2();}
  if((Rturn==0)&&(Hazard==0)){DLR1();}
  bit4state=1;
  bit4=bit4state;
  HighBeam=0;}

if(Lights==2){
  Lights_Radio=1;
  if((Lturn==0)&&(Hazard==0)){DLR2();}
  if((Rturn==0)&&(Hazard==0)){DLR1();}
  bit4state=2;
  bit4=bit4state;}

  if(Lights==3) Lights=0;
  if(Lights==0){
    Lights_Radio=0;
    bit4state=0;
    bit4=bit4state;
    HighBeam=0;
   }

if(button[1].clicks == -1)DLR = !DLR; 
EEPROM.write(0,DLR);

if(DLR == 1){
    beep5_counter = beep5_counter+1;
    beep6_counter = 0;
    if(beep5_counter < 15000)tone(6, 3000);
    if(beep5_counter == 15000)noTone(6);
  }
  if(DLR == 0){
    beep6_counter = beep6_counter+1;
    beep5_counter = 0;
    if(beep6_counter < 15000)tone(6, 3000);
    if(beep6_counter == 15000)noTone(6);
  }

if(button[1].clicks == 2)StrobeFunction = StrobeFunction + 1;
  if(StrobeFunction >= 3)StrobeFunction=0;
EEPROM.write(1,StrobeFunction);
  if(StrobeFunction==0)bit7=2;
  if(StrobeFunction==1)bit7=4;
  if(StrobeFunction==2)bit7=8;

// -------------------------------------------------------------HighBeam

int HighBeam_states;
if(button_states[0] == 0)HighBeam_states =0;
if(button_states[0] == 1)HighBeam_states =4;

if(HighBeam == 0)bit4=bit4state+HighBeam_states;
if(HighBeam == 1)bit4=bit4state+4;

  if(button[0].clicks == -1)HighBeam = !HighBeam;

  if(Lights == 2){
    if(HighBeam == 1){
      beep9_counter = beep9_counter+1;
      beep10_counter = 0;
      if(beep9_counter < 15000)tone(6, 3000);
      if(beep9_counter == 15000)noTone(6);
    }
  }
  if(HighBeam == 0){
    beep10_counter = beep10_counter+1;
    beep9_counter = 0;
    if(beep10_counter < 15000)tone(6, 3000);
    if(beep10_counter == 15000)noTone(6);
  }
  
// -------------------------------------------------------------RPM 
int rpm_state;
int speed_state;
if(button_states[6] == 0)rpm_state =0;
if(button_states[6] == 1)rpm_state =1;

// -------------------------------------------------------------Speed 
if(button_states[9] == 0)speed_state =0;
if(button_states[9] == 1)speed_state =2;
bit5=rpm_state+speed_state;
// ------------------------------------------------------------- Washer
int washer_state;
int horn_state;
if(button_states[5] == 0)washer_state =0;
if(button_states[5] == 1)washer_state =1;

// ------------------------------------------------------------- Horn
if(button_states[3] == 0)horn_state =0;
if(button_states[3] == 1)horn_state =2;

// ------------------------------------------------------------- Wiper
int wiper_state;
if(button[4].clicks == 1) {
  wiper_funtion = !wiper_funtion;
  wiper_funtion1 =0;
  wiper_funtion2 =0;
}

if(button[4].clicks == 2) {
  wiper_funtion1 = !wiper_funtion1;
  wiper_funtion =0;
  wiper_funtion2 =0;
}

if(button[4].clicks == -1) {
  wiper_funtion2 = !wiper_funtion2;
  wiper_funtion =0;
  wiper_funtion1 =0;
}

if((wiper_funtion == 0)&&(wiper_funtion1 == 0)&&(wiper_funtion2 == 0)) wiper_state =0;  //stop

if(wiper_funtion == 1) wiper_state =4;  //speed1

if(wiper_funtion1 == 1) wiper_state =8;  //speed2

if(wiper_funtion2 == 1) wiper_state =16;  //interval;

if(wiper_funtion2 == 1){
    beep7_counter = beep7_counter+1;
    beep8_counter = 0;
    if(beep7_counter < 15000)tone(6, 3000);
    if(beep7_counter == 15000)noTone(6);
  }
  if(wiper_funtion2 == 0){
    beep8_counter = beep8_counter+1;
    beep7_counter = 0;
    if(beep8_counter < 15000)tone(6, 3000);
    if(beep8_counter == 15000)noTone(6);
  }

bit6=washer_state+horn_state+wiper_state;

// ------------------------------------------------------------- 
}
// ------------------------------------------------------------- 
void DLR1(){bit1=2;}
void DLR2(){bit2=2;}
// -------------------------------------------------------------
void hazard(){

  int haz = (millis()/500)%2;

  if(haz == 0) {
    beep2_counter = beep2_counter+1;
    beep1_counter = 0;
    bit0=0;
    if(beep2_counter < 200)tone(6, 3200);
    if(beep2_counter == 200)noTone(6);       
  }
  if(haz == 1) {
    beep1_counter = beep1_counter+1;
    beep2_counter = 0;
    bit0=4;
    if(beep1_counter < 200)tone(6, 3000);
    if(beep1_counter == 200)noTone(6);
  }
}
// ------------------------------------------------------------- 

void lturn(){
  int lt = (millis()/500)%2;
  if(lt == 0) {
    beep2_counter = beep2_counter+1;
    beep1_counter = 0;
    bit0=0;
    if(beep2_counter < 200)tone(6, 3200);
    if(beep2_counter == 200)noTone(6);
  }
  if(lt == 1) {
    beep1_counter = beep1_counter+1;
    beep2_counter = 0;
    bit0=1;
    if(beep1_counter < 200)tone(6, 3000);
    if(beep1_counter == 200)noTone(6);
  }
}
// ------------------------------------------------------------- 

void rturn(){
  int rt = (millis()/500)%2;
  if(rt == 0) {
    beep2_counter = beep2_counter+1;
    beep1_counter = 0;
    bit0=0;
    if(beep2_counter < 200)tone(6, 3200);
    if(beep2_counter == 200)noTone(6);
  }
  if(rt == 1) {
    beep1_counter = beep1_counter+1;
    beep2_counter = 0;
    bit0=2;
    if(beep1_counter < 200)tone(6, 3000);
    if(beep1_counter == 200)noTone(6);
  }
}
// ------------------------------------------------------------- 

