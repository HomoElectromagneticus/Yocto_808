/*
  TR_SEQ.cpp - Library for shift register IN/OUT.
 Created by e-licktronic
 Released into the public domain.
 */

#include "Arduino.h"
#include "TR_SEQ.h"

TR_SEQ SR;

void TR_SEQ:: Initialize()
{
  pinMode(_latchPinIn, OUTPUT);
  pinMode(_clockPin, OUTPUT); 
  pinMode(_dataPinIn, INPUT);
  pinMode(_latchPinOut, OUTPUT);
  pinMode(_dataPinOut,  OUTPUT);
  byte _SR[8]={0};
}

//Modified function run faster
uint8_t TR_SEQ::shiftIn_SRIO() 
{
  /*int i;
  int temp = 0;
  int pinState;
  byte myDataIn = 0;

  for (i=7; i>=0; i--)
  {
    PORTB &= 0<<7;//ClockPin 9 low
    delayMicroseconds(2);
    temp = PINB;//dataPinIn 11
    if (bitRead (temp,6)) {
      pinState = 1;
      myDataIn = myDataIn | (1 << i);
    }
    else {
      pinState = 0;
    }
    PORTB |= 1<<7;//ClockPin 9 high
  }
  return myDataIn;*/
  byte value = 0;

        for (byte i = 0; i <8; i++) {
                value = value | (bitRead(PINB,6)<<(7-i));//value |  (digitalRead(6) << (7-i));
                PORTB |= (1<<7);//digitalWrite(7, HIGH);
                PORTB &= ~(1<<7);// digitalWrite(7, LOW);
        }
        return value;
}

void TR_SEQ::shiftOut_SRIO(uint8_t val)
{
	uint8_t i;

	for (i = 0; i < 8; i++)  {
		
			bitWrite(PORTB,5,!!(val & (1 << (7 - i))));
		PORTB |=  1<<7; //clockPin 9 high
        PORTB &= ~1<<7;//clockPin 9 low
	}
}

void TR_SEQ::Led_All_On()
{
  // turn on all the LEDs
  //Allume toutes les Leds
  PORTB &= 0<<4;//latchPinOut 12 low
  shiftOut_SRIO(255);
  shiftOut_SRIO(255);
  PORTB |= 1<<4;//latchPinOut 12 high
  for(byte i=0;i<8;i++)
  {
  _SR[i]=255;
  }
}

void TR_SEQ::Led_All_Off()
{
  // turn off all the LEDs
  //Eteint toutes les Leds
  PORTB &= 0<<4;//latchPinOut 12 low
  shiftOut_SRIO(0);
  shiftOut_SRIO(0);
  PORTB |= 1<<4;//latchPinOut 12 high
  for(byte i=0;i<8;i++)
  {
  _SR[i]=0;
  }
}
 
void TR_SEQ::Led_SR_Write (byte SR_num, byte data)
{
  // function that sends one octet to the selected shift register
  // Fonction qui envoie un octet au SR selectionné
  _SR[SR_num]=data;
  PORTB &= 0<<4;//latchPinOut 12 low
  
  shiftOut_SRIO(_SR[1]);
  shiftOut_SRIO(_SR[0]);  
  PORTB |= 1<<4;//latchPinOut 12 high
} 

void TR_SEQ:: Led_Step_Write(uint16_t data)
{  
  _SR[0]=data;
  _SR[1]=data>>8;
  // function that sends the state of the LEDS to edit
  //Fonction qui envoi l'état des led pour editer
  PORTB &= 0<<4;//digitalWrite(_latchPinOut, 0);
    shiftOut_SRIO(_SR[3]);
    shiftOut_SRIO(_SR[2]);
    shiftOut_SRIO(data>>8);
    shiftOut_SRIO(data);
  PORTB |= 1<<4;//digitalWrite(_latchPinOut, 1);
}

void TR_SEQ:: ShiftOut_Update(uint16_t data_led,uint16_t data_inst)
{  
  _SR[0]=data_led;
  _SR[1]=data_led>>8;
  _SR[2]=data_inst;
  _SR[1]=data_inst>>8;
  // function that sends the state of the LEDS to edit
  //Fonction qui envoi l'état des led pour editer
  PORTB &= 0<<4;//digitalWrite(_latchPinOut, 0);
    shiftOut_SRIO(data_inst>>8);
    shiftOut_SRIO(data_inst);
    shiftOut_SRIO(data_led>>8);
    shiftOut_SRIO(data_led);
  PORTB |= 1<<4;//digitalWrite(_latchPinOut, 1);
}
void TR_SEQ:: Inst_Send(uint16_t data)
{  
  _SR[2]=data;
  _SR[3]=data>>8;
  // function that sends to the shift register which instrument needs to be triggered
  //Fonction qui envoi au shift register quel instrument doit etre trigger
  PORTB &= 0<<4;//digitalWrite(_latchPinOut, 0);
    shiftOut_SRIO(data>>8);
    shiftOut_SRIO(data);
	shiftOut_SRIO(_SR[1]);
	shiftOut_SRIO(_SR[0]);
  PORTB |= 1<<4;//digitalWrite(_latchPinOut, 1);
}
byte TR_SEQ::Led_SR_Read (byte SR_num)
{ 
  // function that returns the state of the octet to the selected shift register
  // Fonction qui renvoi l'état de l'octet du SR sélectionné
  return _SR[SR_num];
}

void TR_SEQ::Led_Pin_Write (byte Pin_num, byte flag)
{
  // function which sends the state of the LED byte data
  // Fonction qui envoi l'état de le led  byte data;
  bitWrite(_SR[Pin_num/8],Pin_num-(8*(Pin_num/8)),flag);
  PORTB &= 0<<4;//digitalWrite(_latchPinOut, 0);
  shiftOut_SRIO(_SR[1]);
  shiftOut_SRIO(_SR[0]);
  PORTB |= 1<<4;//digitalWrite(_latchPinOut, 1);
}
    
byte TR_SEQ::Button_SR_Read (byte SR_num)
{
  // function that returns in the form of octets the state of the buttons of the
  // selected shift register
  // Fonction qui renvoie sous forme d'octet l'état des boutons du SR sélectionné
  digitalWrite(_latchPinIn,1);
  //delayMicroseconds(20);
  
  _data[0] = shiftIn_SRIO();
  _data[1] = shiftIn_SRIO();
  _data[2] = shiftIn_SRIO();
  
  digitalWrite(_latchPinIn,0);
  return _data[SR_num];
}

byte TR_SEQ::Button_Pin_Read (byte Pin_num){
  // function which allows to test a pin, resent 1 or 0
  // fonction qui permet de tester une PIN, renvoie 1 ou 0
  boolean flag =0; 
  digitalWrite(_latchPinIn,1);
  //delayMicroseconds(20);
  
  _data[0] = shiftIn_SRIO();
  _data[1] = shiftIn_SRIO();
  _data[2] = shiftIn_SRIO();

  digitalWrite(_latchPinIn,0);
  
    if (bitRead (_data[Pin_num/8], Pin_num-(8*(Pin_num/8)) )){
      flag =1;
    }
    else{
      flag =0;
    }
  return flag;
}

int TR_SEQ:: Button_Step_Read()
{
  uint8_t firstbyte;
  uint8_t secondbyte;
  uint16_t word16bits;
  
  digitalWrite(_latchPinIn,1);
  //delayMicroseconds(20);
  
  
  firstbyte =shiftIn_SRIO();
  secondbyte=shiftIn_SRIO();
  
  digitalWrite(_latchPinIn,0);
  
  word16bits = firstbyte | secondbyte <<8;
  return word16bits;
}  
 