
#define NOTE_ON 0x90 //note on sur canal 1


/////////////////////////////////////////////////////////////////////////////////
//Interruption midi en mode midi play
// ce qui est different de l interruption en mode pattern play c'est
//que le shift register des inst est update suivant la note midi recu
/////////////////////////////////////////////////////////////////////////////////
/*void Midi_Synchro_Midi_Play(){ 
 
 if(Serial1.available()>0){
 byte data;
 data = Serial1.read();//enregistre le byte recu dans la variable data
 Serial1.write(data);//echo vers la sorti MIDI OUT de l'entrée MIDI
 switch (data){
 
 //MIDI START
 case 0xfa:
 PORTD |= (1<<5);// met au niveau haut le sorti Din start
 play=1;
 ppqn_count=0;
 first_play=1;
 break;
 
 //MIDI STOP
 case 0xfc:
 PORTD&=~(1<<5);//met a 0 la sorti DIN start
 play=0;
 step_count=0;
 break;
 
 //MIDI CLOCK
 case 0xf8:
 PORTD|=(1<<4);//met a 1 la clock DIN
 delayMicroseconds (2000);//delay neccessaire pour que le cycle de la clock soit correct en DIN_CLK
 PORTD&=~(1<<4);//met a o la clock DIN
 break;
 
 }
 }  
 }*/


void Handle_NoteOn(byte channel, byte pitch, byte velocity)
{
  if (velocity == 0) {
    return;
  }

  if (midi_led_flash_count == 0) {
    PORTC |= MIDI_ACTIVITY_LED;
  }
  midi_led_flash_count = 10;

  //on enregistre le numero du noteOn dans l'instrument selectionné
  if(button_shift){
    inst_midi_note[selected_inst]=pitch;
    inst_midi_note_edited=1;
  }

  if (pitch == inst_midi_note[0]) bitSet(inst_midi_buffer, 0);
  if (pitch == inst_midi_note[1]) bitSet(inst_midi_buffer, 1);
  if (pitch == inst_midi_note[2]) bitSet(inst_midi_buffer, 2);
  if (pitch == inst_midi_note[3]) bitSet(inst_midi_buffer, 3);
  if (pitch == inst_midi_note[4]) bitSet(inst_midi_buffer, 4);
  if (pitch == inst_midi_note[5]) bitSet(inst_midi_buffer, 5);
  if (pitch == inst_midi_note[6]) bitSet(inst_midi_buffer, 6);
  if (pitch == inst_midi_note[7]) bitSet(inst_midi_buffer, 7);
  if (pitch == inst_midi_note[8]) bitSet(inst_midi_buffer, 8);
  if (pitch == inst_midi_note[9]) bitSet(inst_midi_buffer, 9);
  if (pitch == inst_midi_note[10]) bitSet(inst_midi_buffer, 10);
  if (pitch == inst_midi_note[11]) bitSet(inst_midi_buffer, 11);
  if (pitch == inst_midi_note[12]) bitSet(inst_midi_buffer, 12);
  if (pitch == inst_midi_note[13]) bitSet(inst_midi_buffer, 13);
  if (pitch == inst_midi_note[14]) bitSet(inst_midi_buffer, 14);
  if (pitch == inst_midi_note[15]) bitSet(inst_midi_buffer, 15);

  if (inst_midi_buffer > 0 && velocity >= 100) {
    // Enable accent
    bitSet(inst_midi_buffer,0);
  }
}

void Handle_NoteOff(byte channel, byte pitch, byte velocity)
{
}

void Handle_Start()
{
  PORTD |= (1<<5);// met au niveau haut le sorti Din start
  play=1;
  ppqn_count=0;
  first_play=1;
}

void Handle_Stop()
{
  PORTD&=~(1<<5);//met a 0 la sorti DIN start
  play=0;
  step_count=0;
  PORTB &= (~1<<2);// met a 0 la sorti TRIG CPU
}

void Handle_Clock()
{
  Reset_Trig_Out();
  PORTD|=(1<<4);//met a 1 la clock DIN
  delayMicroseconds (2000);//delay neccessaire pour que le cycle de la clock soit correct en DIN_CLK
  PORTD&=~(1<<4);//met a o la clock DIN
  //bitWrite(PORTD,4,!(bitRead (PIND,4)));
  //PORTB &=~1<<2;// met a 0 la sorti TRIG CPU

  //MODE ROLL
  if(roll_mode && ppqn_count%(roll_scale[scale_type][roll_pointer]/4) == 0 && inst_roll>0){//
    PORTB |= (1<<2);//envoie une impulsion sur la sorti trig CPU a chaque pas
    //SR.ShiftOut_Update(temp_step_led,inst_roll);
    //Send_Trig_Out();
  }
  if(step_changed){
    PORTB |= (1<<2);//envoie une impulsion sur la sorti trig CPU a chaque pas
    SR.ShiftOut_Update(temp_step_led,(inst_step_buffer[step_count][pattern_buffer])&(~inst_mute));
    Send_Trig_Out();
    step_changed=0;
  }
  else{
    SR.ShiftOut_Update(temp_step_led,inst_roll);
  }
  ppqn_count++;
  tempo_led_count++;

  //PLAY=================================================================
  if(play){

    //Update clignotement des leds
    if (tempo_led_count>=12){
      tempo_led_count=0;//si le compteur egale un temps on le reinitialise
      tempo_led_flag_block=!tempo_led_flag_block;//clignote au tempo
    }
    if(ppqn_count>=3) tempo_led_flag=0;//on alterne la valeur du flag de la led tempo.
    else tempo_led_flag=1; 

    if (first_play){
      PORTB |= 1<<2;//envoie une impulsion sur la sorti trig CPU a chaque pas
      SR.ShiftOut_Update(temp_step_led,(inst_step_buffer[0][pattern_buffer])&(~inst_mute));
      Send_Trig_Out();
      first_play=0;//initialise le flag
    }
    if (ppqn_count>=pattern_scale[pattern_buffer]/4){
      ppqn_count=0;
      step_count++;
      if (step_count==(nbr_step[pattern_buffer]-1)){
        middle_mesure_flag=1;//indique le mileu de la meusure enfin que la mesure est avancé
        //MODE PATTERN PLAY--------------------------------------------------------------------------------------------------------
        if(selected_mode==PATTERN_MIDI_MASTER || selected_mode==PATTERN_MIDI_SLAVE || selected_mode==PATTERN_DIN_SLAVE){
          pattern_count++;//compte les mesure pour faire avance les pattern en mode song ou quand un block de pattern est selectionner
          if(pattern_count>nbr_pattern_block){//on reset le comteur quand il est superieur au nombre de pattern a lire dans le block
            pattern_count=0;
          }
          if (nbr_pattern_block_changed_A){
            nbr_pattern_block_changed_A=0;
            pattern_count=0;
          }
        }
        //MODE SONG PLAY--------------------------------------------------------------------------------------------------------
        if(selected_mode==SONG_MIDI_MASTER || selected_mode==SONG_MIDI_SLAVE || selected_mode==SONG_DIN_SLAVE){
          pattern_count++;//compte les mesure pour faire avance les pattern en mode song ou quand un block de pattern est selectionner
          if(pattern_count >= total_pattern_song[song_buffer]){//on reset le comteur quand il est superieur au nombre de pattern dans le song
            pattern_count=0;
          }
        }
      }
      step_changed=1;//flag que le pas a change
      if(step_count>=nbr_step[pattern_buffer]){
        step_count=0;
        if(load_pattern_ok){
          pattern_buffer=!pattern_buffer;//permet de switcher entre les deux pattern present dans le buffer au debut de la mesure
          load_pattern_ok=0;
        }
      }
    } 
  }
  //STOP=================================================================
  else if (!play){
    if (tempo_led_count>=12){
      tempo_led_count=0;//si le compteur egale un temps on le reinitialise
      tempo_led_flag=!tempo_led_flag;//on alterne la valeur du flag de la led tempo.
    }
    if(selected_mode==PATTERN_MIDI_MASTER || selected_mode==PATTERN_MIDI_SLAVE || selected_mode==PATTERN_DIN_SLAVE){
      pattern_count=0;//reinitilise le compteur du block pattern
    }
    if((selected_mode==SONG_MIDI_MASTER || selected_mode==SONG_MIDI_SLAVE || selected_mode==SONG_DIN_SLAVE)&&(button_reset)){
      pattern_count=0;//reinitilise le compteur de position du song
    }
  }
    PORTB &= (~1<<2);// met a 0 la sorti TRIG CPU
}



void Check_Midi_Channel()
{
  unsigned int reading = SR.Button_Step_Read();// on lit les boutons
  if(reading!=old_step_button_state){// si ils ont changer on enregistre le temps
    millis_debounce_step_button=millis();
  }
  if((millis()-millis_debounce_step_button)>=DEBOUNCE){
    if(reading!=step_button_state){
      step_button_state=reading;
      if(!button_shift){
        for (byte i=0;i<16;i++){//loop autant de fois que de bouton step soit 16
          if (bitRead (step_button_state,i)){
            //on retourne la valeur du pattern selectionner
            selected_channel=i;
            selected_channel_changed=1;//flag que le pattern selectionner a change
            temp_step_led= (1<<selected_channel);
            break;
          }
        }
      }
    }
  }
  old_step_button_state=reading;
}

void Disconnect_Callback()
{
  /*  
   DDRA |= ~B11111100;// les PIN 39 et 40 sont en sorti pour le commun des rotary switch les autres sont en entree pour l'encoder et les rotary switch
   DDRC |= B11111100;// les 6 bits fort du PORTC sont en sorti pour les 6 led Edit
   DDRB |= B111;// les trois sorties TRIG  CPU,1 et 2
   DDRD |= (1<<6);//sortie trig out
   inst_midi_buffer=0;
   SR.ShiftOut_Update(temp_step_led,inst_midi_buffer);
   PORTB &=~1<<2;
   */
  PORTC &= ~(B11111100);//clear les edits leds dans ce mode 
  SR.Led_Step_Write(0);//tous les leds Step Off
  MIDI.disconnectCallbackFromType(NoteOn);
  MIDI.disconnectCallbackFromType(Clock);
  MIDI.disconnectCallbackFromType(Start);
  MIDI.disconnectCallbackFromType(Stop);
}


