#include <Metro.h>
#include <FlexCAN.h>
#include <EEPROM.h>
#include <Bounce.h>
#include "Arduino.h"
#include "lin.h"

Metro cansend = Metro(20);// milliseconds

static CAN_message_t msg0;

#define RED 0x0
#define BLUE 0x1

#define interval 4000
#define send_interval 40

byte counter = 0x00; 
byte state = 0xdf;
byte sp = 0x00; 
int counter0 = 0;
unsigned long waitUntil = 0;

int hood = 5;
int brake = 0;

Lin lin;

int led0 = 20;
int led1 = 19;
int led2 = 18;
int led3 = 8;
int led4 = 21;
int led5 = 2;
int led6 = 1;
int led7 = 7;
int led8 = 22;
int led9 = 23;
int washer = 11;
int horn = 12;

int strobe_status=0;
static CAN_message_t rxmsg;

byte whichLED = RED;
byte Red_State = LOW;
byte Blue_State = LOW;

unsigned long switchDelay = 400;
unsigned long strobeDelay = 50;
unsigned long strobeWait = 0;
unsigned long waitUntilSwitch = 0;//(millis()-switchDelay);
// -------------------------------------------------------------
void setup(){
 Can0.begin(100000);
 lin.begin(19200);

 pinMode(led0, OUTPUT);
 pinMode(led1, OUTPUT);
 pinMode(led2, OUTPUT);
 pinMode(led3, OUTPUT);
 pinMode(led4, OUTPUT);
 pinMode(led5, OUTPUT);
 pinMode(led6, OUTPUT);
 pinMode(led7, OUTPUT);
 pinMode(led8, OUTPUT);
 pinMode(led9, OUTPUT);
 pinMode(horn, OUTPUT);
 pinMode(washer, OUTPUT);
 pinMode(hood,INPUT);
 pinMode(brake,INPUT);

 digitalWrite(led0, 0);
 digitalWrite(led1, 0);
 digitalWrite(led2, 0);
 digitalWrite(led3, 0);
 digitalWrite(led4, 0);
 digitalWrite(led5, 0);
 digitalWrite(led6, 0);
 digitalWrite(led7, 0);
 analogWrite(led8, 0);
 analogWrite(led9, 0);
 digitalWrite(horn, 0);
 digitalWrite(washer, 0);
 digitalWrite(hood,HIGH);
 digitalWrite(brake,HIGH);
}

//--------------------------------------------------------------

void sendstop ()
{
  if (counter0 == 0){
    uint8_t stop1[] = { 0x00, 0x00, 0x00, 0x00, 0x00, counter, 0x00, 0x00 };
    lin.send(0x47, stop1,8,2);
    if ((unsigned long)(millis() - waitUntil) >= send_interval) {
    lin.send(0x47, stop1,8,2);
    }
  }
   else{
    uint8_t stop1[] = { 0x1f, 0xef, 0xca, 0xff, 0x28, counter, 0x00, 0x00 };
    if ((unsigned long)(millis() - waitUntil) >= send_interval) {
    lin.send(0x47, stop1,8,2);
    counter0 = 1;
    }
   } 
}
//--------------------------------------------------------------

void sendspeed1 ()
{
 uint8_t speed1[] = { 0x3f, 0xef, 0xda, 0xff, 0x28, counter, 0x14, 0x00 }; 
 if ((unsigned long)(millis() - waitUntil) >= send_interval) {
 lin.send(0x47, speed1,8,2);
 } 
}

void sendspeed2 ()
{
 uint8_t speed2[] = { 0x3f, 0xef, 0xda, 0xff, 0x28, counter, 0x1c, 0x00 };
 if ((unsigned long)(millis() - waitUntil) >= send_interval) {
 lin.send(0x47, speed2,8,2);
 }
}
//--------------------------------------------------------------

void sendinterval1 ()
{ 
 uint8_t interval1[] = { 0x1f, 0xef, 0xca, 0xff, 0x28, counter, 0x00, 0x00 };
 if ((unsigned long)(millis() - waitUntil) >= send_interval) {
 lin.send(0x47, interval1,8,2);
 }
 if ((unsigned long)(millis() - waitUntil) >= interval) {
  uint8_t interval1[] = { 0x3f, 0xef, 0xda, 0xff, 0x28, counter, 0x14, 0x00 };
  if ((unsigned long)(millis() - waitUntil) >= send_interval) {
  lin.send(0x47, interval1,8,2);
  }
  if ((unsigned long)(millis() - waitUntil) >= send_interval) {
  lin.send(0x47, interval1,8,2);
  waitUntil = waitUntil + interval;
  }
 }
}
             
//--------------------------------------------------------
void loop(){
  
counter = counter + 0x10;
 if (counter == 0xf0)
 {
  counter = 0x00;
 }  

if ( Can0.read(rxmsg) ) {
       
      switch (rxmsg.id) { 
       case 150:

        
        //*******************************Turn/Hazard
        if (rxmsg.buf[0]==0){
        digitalWrite(led4,0);  
        digitalWrite(led5,0);
        digitalWrite(led6,0);  
        digitalWrite(led7,0);
      }
        if (rxmsg.buf[0]==1){
        digitalWrite(led4,0);  
        digitalWrite(led6,0);
        digitalWrite(led5,1);
        digitalWrite(led7,1);
      }
        if (rxmsg.buf[0]==2){  
        digitalWrite(led5,0);  
        digitalWrite(led7,0);
        digitalWrite(led4,1);
        digitalWrite(led6,1);
      }
        if (rxmsg.buf[0]==4){
        digitalWrite(led4,1);  
        digitalWrite(led5,1);
        digitalWrite(led7,1);
        digitalWrite(led6,1);
      }
      //*******************************DLR_1

        if (rxmsg.buf[1]==0){
        analogWrite(led8,0);          
      }
        if (rxmsg.buf[1]==1){
        analogWrite(led8,254);  
      }
        if (rxmsg.buf[1]==2){
        analogWrite(led8,20);  
      }

      //*******************************DLR_2

        if (rxmsg.buf[2]==0){
        analogWrite(led9,0);  
      }
        if (rxmsg.buf[2]==1){
        analogWrite(led9,254);  
      }
        if (rxmsg.buf[2]==2){
        analogWrite(led9,20);  
      }

      //*******************************Cruze Lights/LowBeam

        if (rxmsg.buf[4]==0){
        digitalWrite(led2,0);  
        digitalWrite(led3,0);
      }
        if (rxmsg.buf[4]==2){
        digitalWrite(led2,1);  
        digitalWrite(led3,1);
      }

      //*******************************HighBeam
        int hibeam_status =bitRead(rxmsg.buf[4],2);
        if (hibeam_status ==0){
        digitalWrite(led0,0);  
        digitalWrite(led1,0);
      }
        if (hibeam_status ==1){
        digitalWrite(led0,1);  
        digitalWrite(led1,1);
      }

     //*******************************Winshield
        int wiper_stop_speed1 = bitRead(rxmsg.buf[6],2);
        int wiper_speed2 = bitRead(rxmsg.buf[6],3);
        int wiper_interval=bitRead(rxmsg.buf[6],4);
        if ((wiper_stop_speed1 == 0)&&(wiper_speed2 == 0)&& (wiper_interval == 0))sendstop ();
        if (wiper_stop_speed1 == 1)sendspeed1 ();
        if (wiper_speed2 == 1)sendspeed2 ();
        if (wiper_interval == 1)sendinterval1 ();
      //*******************************Horn/Washer
        int horn_status=bitRead(rxmsg.buf[6],1);
        int washer_status=bitRead(rxmsg.buf[6],0);
        if (horn_status==0){
        digitalWrite(horn,0); 
      }
        if (washer_status==0){
        digitalWrite(washer,0); 
      }
        if (horn_status==1){
        digitalWrite(horn,1);  
      }
        if (washer_status==1){
        digitalWrite(washer,1); 
      }
      //*******************************Strobe
        strobe_status=bitRead(rxmsg.buf[3],1);
        if (strobe_status==1){
          strobe();
        }
      //*******************************
      break;
      }
    }

    int hood_status=!digitalRead(hood);
    int brake_status=!digitalRead(brake);

    if (cansend.check() == 1) {

    msg0.len = 8;
    msg0.id = 1976;
    msg0.buf[0] = hood_status;
    msg0.buf[1] = brake_status;
    msg0.buf[2] = 0;
    msg0.buf[3] = 0;
    msg0.buf[4] = 0;
    msg0.buf[5] = 0;
    msg0.buf[6] = 0;
    msg0.buf[7] = 0;
    Can0.write(msg0);
    }
}

// ------------------------------------------------------------- 

void strobe(){

  int StrobeFunction1=bitRead(rxmsg.buf[7],1);
  int StrobeFunction2=bitRead(rxmsg.buf[7],2);
  int StrobeFunction3=bitRead(rxmsg.buf[7],3);
  if(StrobeFunction1==1){
    digitalWrite(led8,0);
    digitalWrite(led9,0);
    digitalWrite(led6,Red_State);
    digitalWrite(led7,Blue_State);}

  if(StrobeFunction2==1){
    digitalWrite(led6,0);
    digitalWrite(led7,0);
    digitalWrite(led9,Red_State);
    digitalWrite(led8,Blue_State);
    }
     
  if(StrobeFunction3==1){
    digitalWrite(led6,Red_State);
    digitalWrite(led7,Blue_State);
    digitalWrite(led9,Red_State);
    digitalWrite(led8,Blue_State);}
    
  unsigned long waitUntilSwitch0 = millis(); 
    if(waitUntilSwitch0 - waitUntilSwitch > switchDelay) {
        waitUntilSwitch = waitUntilSwitch0;
        Red_State = 0;
        Blue_State = 0;
        whichLED = !whichLED;  // toggle LED to strobe
        //waitUntilSwitch += switchDelay;//switchDelay;
    }
 
    // Create the strobing effect
   unsigned long strobeWait0 = millis(); 
    if(strobeWait0 - strobeWait > strobeDelay) {
        strobeWait = strobeWait0;
       if (whichLED == RED)
       Red_State = !Red_State;
       if (whichLED == BLUE)
       Blue_State = !Blue_State;
    }
}
