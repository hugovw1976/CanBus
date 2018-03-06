#include <Metro.h>
#include <FlexCAN.h>
#include <EEPROM.h>
#include <Bounce.h>
#define RED 0x0
#define BLUE 0x1
Metro cansend = Metro(20);// milliseconds

static CAN_message_t msg0;
static CAN_message_t msg7;

int led1 = 1;
int led2 = 2;
int led7 = 7;
int led8 = 8;
int led11 = 11;
int led12 = 12;
int led18 = 18;
int led19 = 19;
int led20 = 20;
int led21 = 21;
int led22 = 22;
int led23 = 23;
int brake=32;
int rain_status=0;
int reverse;
int rev = 0;
int trunk = 5;

int strobe_status=0;
static CAN_message_t rxmsg;

unsigned long previousMillis = 0; 

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
 pinMode(led1, OUTPUT);
 pinMode(led2, OUTPUT);
 pinMode(led7, OUTPUT);
 pinMode(led8, OUTPUT);
 pinMode(led11, OUTPUT);
 pinMode(led12, OUTPUT);
 pinMode(led18, OUTPUT);
 pinMode(led19, OUTPUT);
 pinMode(led20, OUTPUT);
 pinMode(led21, OUTPUT);
 pinMode(led22, OUTPUT);
 pinMode(led23, OUTPUT);
 pinMode(rev,INPUT);
 pinMode(trunk,INPUT);
 
 digitalWrite(led1, 0);
 digitalWrite(led2, 0);
 digitalWrite(led7, 0);
 digitalWrite(led8, 0);
 digitalWrite(led11, 0);
 digitalWrite(led12, 0);
 digitalWrite(led18, 0);
 digitalWrite(led19, 0);
 digitalWrite(led20, 0);
 digitalWrite(led21, 0);
 digitalWrite(led22, 0);
 digitalWrite(led23, 0);
 digitalWrite(rev,HIGH);
 digitalWrite(trunk,HIGH);

}               
//--------------------------------------------------------
void loop(){

  int trunk_status=!digitalRead(trunk); 
  brake=32; //Hand Brake 0-off 2-on

if ( Can0.read(rxmsg) ) {
  
    switch (rxmsg.id) { 
       case 150:{
        //*******************************Turn/Hazard
        if (rxmsg.buf[0]==0){
        digitalWrite(led7,0);  
        digitalWrite(led21,0);
      }
        if (rxmsg.buf[0]==1){
        digitalWrite(led7,0);  
        digitalWrite(led21,1);
      }
        if (rxmsg.buf[0]==2){
        digitalWrite(led21,0);  
        digitalWrite(led7,1);
      }
        if (rxmsg.buf[0]==4){
        digitalWrite(led7,1);  
        digitalWrite(led21,1);
      }
      //*******************************Cuartos
        if (rxmsg.buf[4]==0){
        digitalWrite(led1,0);
        digitalWrite(led19,0); 
        digitalWrite(led23,0);          
      }
        if ((rxmsg.buf[4]==1)||(rxmsg.buf[4]==2)){
        digitalWrite(led1,1);
        digitalWrite(led19,1);
        digitalWrite(led23,1);   
      }
      //*******************************Reverse
        int reverse_status=!digitalRead(rev);
        digitalWrite(led8,reverse_status);
        digitalWrite(led20,reverse_status);
        if (reverse_status == 0) reverse = 0;
        if (reverse_status == 1) reverse = 2;

       //*******************************3Stop
        
        rain_status=bitRead(rxmsg.buf[3],0);
        if(rain_status==1){
        rain();          
        }
      //*******************************Strobe
        
        strobe_status=bitRead(rxmsg.buf[3],1);
        if (strobe_status==1)strobe();
       }
      break; 
      case 1976:{
      //*******************************Brake
        
        int brake_status=rxmsg.buf[1];
        if(brake_status == 0){
        digitalWrite(led2,0);
        digitalWrite(led22,0);
        if(rain_status==0){
        digitalWrite(led12,0);           
        }  
        }       
        if(brake_status == 1){
        digitalWrite(led2,1);
        digitalWrite(led22,1); 
        if(rain_status==0){
        digitalWrite(led12,1);           
        }
       }
      }
      break;
      //*************************************
    }
  }
    
    
    if (cansend.check() == 1) {
    msg7.len = 8;
    msg7.id = 849;
    msg7.buf[0] = reverse;
    msg7.buf[1] = 0;
    msg7.buf[2] = brake;
    msg7.buf[3] = 0;
    msg7.buf[4] = 0;
    msg7.buf[5] = 0;
    msg7.buf[6] = 0;
    msg7.buf[7] = 0;
    Can0.write(msg7);

    msg0.len = 8;
    msg0.id = 1977;
    msg0.buf[0] = trunk_status;
    msg0.buf[1] = 0;
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
void rain(){
  int ra = (millis()/100)%2;
  if(ra == 0) digitalWrite(led12,0);
  if(ra == 1) digitalWrite(led12,1);
}
// ------------------------------------------------------------- 
void strobe(){

  int StrobeFunction1=bitRead(rxmsg.buf[7],1);
  int StrobeFunction2=bitRead(rxmsg.buf[7],2);
  int StrobeFunction3=bitRead(rxmsg.buf[7],3);
  if(StrobeFunction1==1){
    digitalWrite(led8,0);
    digitalWrite(led20,0);
    digitalWrite(led21,Red_State);
    digitalWrite(led7,Blue_State);}

  if(StrobeFunction2==1){
    digitalWrite(led7,0);
    digitalWrite(led21,0);
    digitalWrite(led8,Red_State);
    digitalWrite(led20,Blue_State);
    }
     
  if(StrobeFunction3==1){
    digitalWrite(led21,Red_State);
    digitalWrite(led7,Blue_State);
    digitalWrite(led8,Red_State);
    digitalWrite(led20,Blue_State);}
    
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
