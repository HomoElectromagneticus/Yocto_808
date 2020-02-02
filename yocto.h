/*

yocto.h

This header file is used to make the Yocto source code compatible with
Arduino.mk. Normally, when using the Arduino IDE, these declarations
are created by an Arduino pre-processor. The declarations below were
made by hand.

This file was made for Yocto source v1.0c. It may have to be changed
for future versions.

*/

#ifndef yocto_h
#define yocto_h

void Reset_Song();
void Verticalize_Pattern();
void Set_CPU_Trig_High();
void Set_CPU_Trig_Low();
void Send_Trig_Out(int trig_outs);
void Reset_Trig_Out();
void Handle_NoteOff(byte channel, byte pitch, byte velocity);
void Check_Midi_Channel();
void Send_Trig_Out_Midi();
void Handle_NoteOn(byte channel, byte pitch, byte velocity);
void Mode_Synchro(byte mode);
void Set_Dinsync_Clock_High();
void Set_Dinsync_Clock_Low();
void Set_Dinsync_Run_High();
void Set_Dinsync_Run_Low();
void Disconnect_Callback();
void Handle_Stop();
void Handle_Clock();
void Handle_Start();
void Mode_Song_Play();
void Update_Song_Led();
void calc_CRC8();
void Update_Pattern_EEprom();
void Update_Song_EEprom();
void Dump_EEprom();
void Receive_EEprom(byte * array, unsigned size);
void Mode_Song_Edit();
void Paste_Pattern();
void Copy_Pattern();
void Clear_Pattern();
void Update_Pattern_Led();
void Mode_Pattern();
void MIDI_Send(byte OutByte);
void Wire_Begin_TX(uint16_t address);
void Save_Midi_Channel();
void Save_Midi_Note();
void Verticalize_Pattern_After_Cleared();
void Load_Pattern();
int8_t Read_Encoder();

#endif