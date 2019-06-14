/////////////////////////////////
//Fonction des sorties Trig
/////////////////////////////////

void Reset_Trig_Out() {
  // set all of the trig output pins high
  PORTB |= 0b11;        //TRIG1 and TRIG2, the port must be written all at once
  PORTD |= (1 << 6);    //TRIG3
}

void Send_Trig_Out(int trig_outs) {
  // set the trig output pins low to send a trigger
  if (bitRead(trig_outs, TRIG1)) digitalWrite(TRIG1_PIN, LOW);
  if (bitRead(trig_outs, TRIG2)) digitalWrite(TRIG2_PIN, LOW);
  if (bitRead(trig_outs, TRIG3)) PORTD &= -(1 << 6);
}

void Send_Trig_Out_Midi() {
  // set the trig output pins low to send a trigger (from MIDI)
  if (bitRead(inst_midi_buffer,TRIG1)) digitalWrite(TRIG1_PIN,LOW);
  if (bitRead(inst_midi_buffer,TRIG2)) digitalWrite(TRIG2_PIN,LOW);
  if (bitRead(inst_midi_buffer,TRIG3)) PORTD &= -(1 << 6);;
}
