#ifndef TR_SEQ_h     
#define TR_SEQ_h
#include "Arduino.h"


#define _latchPinOut 4
#define _dataPinOut 5
#define _latchPinIn 3
#define _dataPinIn  6
#define _clockPin 7


class TR_SEQ
{
public:
  void Initialize();
  uint8_t shiftIn_SRIO();
  void shiftOut_SRIO(uint8_t val);
  void Led_SR_Write (byte SR_num, byte data);
  void Led_Pin_Write (byte Pin_num, byte flag);
  void Led_Step_Write(uint16_t data);
  void Led_All_On();
  void Led_All_Off();
  void Inst_Send(uint16_t data);
  void ShiftOut_Update(uint16_t data_led, uint16_t data_inst);
  byte Led_SR_Read (byte SR_num);
  byte Button_SR_Read (byte SR_num);
  byte Button_Pin_Read (byte Pin_num);
  int Button_Step_Read();
private:
  byte _SR[];
  byte _data[3];
};

extern TR_SEQ SR;

#endif // Fin si
