/////////////////////////////////
//Fonction des sorties Trig
/////////////////////////////////

void Reset_Trig_Out()
{
  digitalWrite(TRIG1_PIN, HIGH);
  digitalWrite(TRIG2_PIN, HIGH);
  digitalWrite(TRIG3_PIN, HIGH);
}

void Send_Trig_Out()
{
  if (bitRead((inst_step_buffer[step_count][pattern_buffer])&(~inst_mute),TRIG1)) digitalWrite(TRIG1_PIN,LOW);
  if (bitRead((inst_step_buffer[step_count][pattern_buffer])&(~inst_mute),TRIG2)) digitalWrite(TRIG2_PIN,LOW);
  if (bitRead((inst_step_buffer[step_count][pattern_buffer])&(~inst_mute),TRIG3)) digitalWrite(TRIG3_PIN,LOW);
}

void Send_Trig_Out_Midi()
{
  if (bitRead(inst_midi_buffer,TRIG1)) digitalWrite(TRIG1_PIN,LOW);
  if (bitRead(inst_midi_buffer,TRIG2)) digitalWrite(TRIG2_PIN,LOW);
  if (bitRead(inst_midi_buffer,TRIG3)) digitalWrite(TRIG3_PIN,LOW);
}

/*void Test_Trig_Out()
 {
 PORTB |= ((1<<1)|(1<<0));
 TRIG1_PORT |= (1<<6);
 }*/


