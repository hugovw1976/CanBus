#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include <avr/interrupt.h>

#define ARDUINOPIN2 2
#define ARDUINOPIN3 3
#define ARDUINOPIN5 5
#define ARDUINOPIN6 6
#define ARDUINOPIN7 7
#define ARDUINOPIN8 8
#define ARDUINOPIN16 16
#define ARDUINOPIN17 17
#define ARDUINOPIN18 18
#define ARDUINOPIN19 19

int LED1 = A0;//14
int LED2 = A1;//15

const byte CHIP_ENABLE = 9;
const byte CHIP_SELECT = 10;

// Pins on the remote for buttons
const uint8_t button_pins[] = { 2,3,5,6,7,8,16,17,18,19 };
const uint8_t num_button_pins = sizeof(button_pins);

const uint64_t pipe = 0xE7E7E7E7E7LL;

uint8_t button_states[num_button_pins];


ISR(PCINT0_vect)
{
  //This vector is only here to wake unit up from sleep mode
}

ISR(PCINT1_vect)
{
  //This vector is only here to wake unit up from sleep mode
}

ISR(PCINT2_vect)
{
  //This vector is only here to wake unit up from sleep mode
}

void setup(void)
{ 
  pinMode(LED1,OUTPUT);
  pinMode(LED2,OUTPUT);
  digitalWrite(LED1,LOW);
  digitalWrite(LED2,LOW);
  
    int i = num_button_pins;
    while(i--)
    {
      pinMode(button_pins[i],INPUT);
      digitalWrite(button_pins[i],HIGH);
    }
  PCMSK0 = (1<<PCINT0);
  PCMSK1 = (1<<PCINT10)|(1<<PCINT11)|(1<<PCINT12)|(1<<PCINT13);
  PCMSK2 = (1<<PCINT18)|(1<<PCINT19)|(1<<PCINT21)|(1<<PCINT22)|(1<<PCINT23);
  PCICR = (1<<PCIE0) | (1<<PCIE1) | (1<<PCIE2); //Enable Pin Change Interrupt Request
  PCIFR = (1<<PCIF0) | (1<<PCIF1) | (1<<PCIF2); //Enable the Pin Change interrupts to monitor button presses  
}


void sendremote ()
  {
  // Set up nRF24L01 radio on SPI bus plus pins 9 & 10
  
  RF24 radio(CHIP_ENABLE, CHIP_SELECT);
  
  digitalWrite (SS, HIGH);
  SPI.begin ();
  digitalWrite (CHIP_ENABLE, LOW); 
  digitalWrite (CHIP_SELECT, HIGH);
  
  //
  // Setup and configure rf radio
  //
  radio.begin();
  radio.setChannel(5);
  radio.setDataRate(RF24_250KBPS);
  radio.setRetries(10,10);
  radio.setPALevel(RF24_PA_MAX);
  radio.openWritingPipe(pipe);

  // Take the voltage.  This will block until complete
 
  ADCSRA =  bit (ADEN);   // turn ADC on
  ADCSRA |= bit (ADPS0) |  bit (ADPS1) | bit (ADPS2);  // Prescaler of 128
  ADMUX = bit (REFS0) | bit (MUX3) | bit (MUX2) | bit (MUX1);
  
  bitSet (ADCSRA, ADSC);  // start a conversion  
  while (bit_is_set(ADCSRA, ADSC))
    { }
 
  // and again, <sigh>
  bitSet (ADCSRA, ADSC);  // start a conversion  
  while (bit_is_set(ADCSRA, ADSC))
    { }
  
  unsigned int reading = ADC;
  float battery = 1.086/(reading+0.5)*1024.0;

  int i = num_button_pins;
  bool different = false;
    while(i--)
    {
      uint8_t state = ! digitalRead(button_pins[i]);
      if ( state != button_states[i] )
      {
        different = true;
        button_states[i] = state;        
      }
    }
    
    if (different){
      bool ok = radio.write( button_states, num_button_pins );
      if(battery >= 2.6){
       digitalWrite(LED1,HIGH);
       delay(20);
      }
      else{
       digitalWrite(LED2,HIGH);
       delay(20);
      }
    }

  radio.powerDown ();
  
  SPI.end ();
  // set pins to OUTPUT and LOW  
  for (byte i = 9; i <= 13; i++)
    {
    pinMode (i, OUTPUT);    
    digitalWrite (i, LOW); 
    } 
  }

void loop(void)
{    
    sendremote();
    digitalWrite(LED1,LOW);
    digitalWrite(LED2,LOW);
    ADCSRA &= ~(1<<7);
    SMCR |=(1<<2);
    SMCR |=1;
    MCUCR |= (3<<5);
    MCUCR = (MCUCR & ~(1<<5)) | (1<<6);
    __asm__  __volatile__("sleep");
}
