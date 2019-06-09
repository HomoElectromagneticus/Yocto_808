///////////////////////////////////////////////////////////////////////
// Timer qui incremente le compteur a 96PPQN
// en mode master
//////////////////////////////////////////////////////////////////////

void Count_96PPQN()
{

  //  if ( selected_mode == MIDI_PLAY )  return;



  //Set_CPU_Trig_Low();
  Reset_Trig_Out();

  //MODE ROLL
  if(roll_mode && ppqn_count%roll_scale[scale_type][roll_pointer] == 0 && inst_roll>0){
    Set_CPU_Trig_High();
    Send_Trig_Out();
  }
  if(step_changed){
    SR.ShiftOut_Update(temp_step_led,((inst_step_buffer[step_count][pattern_buffer])&(~inst_mute)|inst_roll));
    step_changed=0;
    Set_CPU_Trig_High();
  }
  else{
    SR.ShiftOut_Update(temp_step_led,inst_roll);
  }

  ppqn_count++;//incremente le compteur PPQN96
  tempo_led_count++;//incremente le compteur pour la led de tempo

  //PLAY=================================================================
  if (play){

    // update the flashing of the LEDs
    //update le clignotement des leds
    if (tempo_led_count>=48){
      // if the counter equals a tempo, you reinitialize it
      tempo_led_count=0;//si le compteur egale un temps on le reinitialise
      // flash the tempo LED
      tempo_led_flag_block=!tempo_led_flag_block;//clignote au tempo
    }
    // we alternate the value of the flag of the LED tempo
    if(ppqn_count>=12) tempo_led_flag=0;//on alterne la valeur du flag de la led tempo.
    else tempo_led_flag=1; 

    //is it the start?
    //est ce le demarrage ?
    if(first_play){
      MIDI_Send(0xfa);//Serial1.write(0xfa);//Midi Start
      ppqn_count=0;//initialise le compteur PPQN
      Set_Dinsync_Run_High();
      Set_Dinsync_Clock_Low();
      Send_Trig_Out();
      first_play=0;
    }
    // the first note after the push on start needs to be shifted over / delayed one pulse
    // the PPQN needs to be savvy (?) with the first note of the machine in SLAVE DIN mode
    //la premiere note apres l'appuie sur start doit etre decale d'une pulsation 
    //de PPQN pour etre calée avec la premiere note de la machine en SLAVE DIN
    if (first_play_A && (ppqn_count==1)){
      Set_CPU_Trig_High();
      SR.ShiftOut_Update(temp_step_led,(inst_step_buffer[0][pattern_buffer])&(~inst_mute));
      first_play_A=0;//initialise le flag
    }
    else if (ppqn_count>=pattern_scale[pattern_buffer]){
      ppqn_count=0;
      step_count++;
      if (step_count==(nbr_step[pattern_buffer]-1)){
        middle_mesure_flag=1;
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
        end_mesure_flag=1;
        //------------------------------------------------
        if(load_pattern_ok){//si le pattern a bien ete loader
          pattern_buffer=!pattern_buffer;//on switche entre les deux pattern present dans le buffer au debut de la mesure
          load_pattern_ok=0;//reinitialise le flag de load pattern
        }
      }
    } 
  }

  //STOP=================================================================
  else if(!play){
    if(first_stop){
      MIDI_Send(0xfc);//Serial1.write(0xfc);//Midi Stop
      first_stop=0;
    }
    if(selected_mode==PATTERN_MIDI_MASTER || selected_mode==PATTERN_MIDI_SLAVE || selected_mode==PATTERN_DIN_SLAVE){
      pattern_count=0;//reinitilise le compteur
    }
    if((selected_mode==SONG_MIDI_MASTER || selected_mode==SONG_MIDI_SLAVE || selected_mode==SONG_DIN_SLAVE)&&(button_reset)){
      pattern_count=0;//reinitilise le compteur
    }
    step_count=0;//reinitialise a 0 le step compteur
    debut_mesure_count=0;
    Set_Dinsync_Run_Low();
    if (tempo_led_count>=48){
      tempo_led_count=0;//si le compteur egale un temps on le reinitialise
      tempo_led_flag=!tempo_led_flag;//on alterne la valeur du flag de la led tempo.
    }
  }
  //incrementer la clock de la synchro DIN tous les 4 PPQN96 pour avoir PPQN24
  if((ppqn_count%2)==1){ //modulo de 2 = 1 car la clock PPQN et la clock din sont decaler d'un demi cylce 
    bitWrite(PORTD,4,!(bitRead (PIND,4)));
  }
  //Incrementer la clock Midi tous les 4 PPQN96 pour avoir PPQN24
  if((ppqn_count%4)==1){
    MIDI_Send(0xf8);//Serial1.write (0xf8);
  }

  for (int tempx=0; tempx<500; tempx++)
  {
  }//tempo pour que la pulse CPU soit egal a 1ms

  Set_CPU_Trig_Low();

}

///////////////////////////////////////////////////////////////////////
// Timer for 4ms dinsync output
// in mode master and midi sync.
//////////////////////////////////////////////////////////////////////
void Count_Clock() {
  if (sync_mode == MASTER || sync_mode == MIDI_SLAVE) {
    if (dinsync_first_clock_timeout != 0) {
      dinsync_first_clock_timeout--;
      if (dinsync_clock_timeout == 0) {
        Set_Dinsync_Clock_High();
      }
    }

    if (dinsync_clock_timeout != 0) {
      dinsync_clock_timeout--;
      if (dinsync_clock_timeout == 0) {
        Set_Dinsync_Clock_Low();
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////
//  Pin Change interruption
// Une interruption est generer sur les Pin PD4 et PD5
// PD4= Din CLK      PD5= Din Start
// a la sync Master ici on est en 24PPQN et non en 96PPQN
///////////////////////////////////////////////////////////////////////
ISR (PCINT3_vect)
{
  Reset_Trig_Out();
  //PLAY=================================================================
  if(PIND & (1<<4) && play){
    Set_CPU_Trig_Low();

    //MODE ROLL
    if(roll_mode && ppqn_count%(roll_scale[scale_type][roll_pointer]/4) == 0 && inst_roll>0){//
      Set_CPU_Trig_High();
      //SR.ShiftOut_Update(temp_step_led,inst_roll);
      //Send_Trig_Out();
    }
    //Si le pas a changer
    if(step_changed){
      SR.ShiftOut_Update(temp_step_led,inst_step_buffer[step_count][pattern_buffer]&(~inst_mute));
      Send_Trig_Out();
      step_changed=0;
    }
    else{
      SR.ShiftOut_Update(temp_step_led,inst_roll);
    }
    //pdate le clignotements des leds
    if (tempo_led_count>=12) {
      tempo_led_count=0;//si le compteur egale un temps on le reinitialise
      tempo_led_flag_block=!tempo_led_flag_block;//clignote au tempo
    }
    if(ppqn_count>=3) tempo_led_flag=0;//on alterne la valeur du flag de la led tempo.
    else tempo_led_flag=1; 

    //Si c'est le premier pas apres le play
    if(first_play){
      MIDI_Send(0xfa);//Serial1.write(0xfa);//Midi Start  
      ppqn_count=0;
      Set_CPU_Trig_High();
      SR.ShiftOut_Update(temp_step_led,inst_step_buffer[0][pattern_buffer]&(~inst_mute));
      Send_Trig_Out();
      first_play=0;
    }
    MIDI_Send(0xf8);//Serial1.write (0xf8);//MIDI CLOCK Tick
    ppqn_count++;     //incremente compteur PPQN
    tempo_led_count++;//incremente le compteur pour la led de tempo

    // Si le compteur PPQN egal la scale selectionner
    if (ppqn_count==(pattern_scale[pattern_buffer]/4)){
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
      Set_CPU_Trig_High();
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
  else if(PIND & (1<<4) && !play){
    if(first_stop){
      MIDI_Send(0xfc);//Serial1.write(0xfc);//Midi Stop
      first_stop=0;
    }
    if(selected_mode==PATTERN_MIDI_MASTER || selected_mode==PATTERN_MIDI_SLAVE || selected_mode==PATTERN_DIN_SLAVE){
      pattern_count=0;//reinitilise le compteur
    }
    if((selected_mode==SONG_MIDI_MASTER || selected_mode==SONG_MIDI_SLAVE || selected_mode==SONG_DIN_SLAVE)&&(button_reset)){
      pattern_count=0;//reinitilise le compteur
    }
    MIDI_Send(0xf8);//Serial1.write (0xf8);
    step_count=0;//reinitialise a -1 le compteur de pas pour pouvoir lire le premier pas 
    ppqn_count=0;
    SR.ShiftOut_Update(temp_step_led,0);
    tempo_led_count++;//incremente le compteur pour la led de tempo
    if (tempo_led_count>=12){
      tempo_led_count=0;//si le compteur egale un temps on le reinitialise
      tempo_led_flag=!tempo_led_flag;//on alterne la valeur du flag de la led tempo.
    }
  }
}

void MIDI_Send(byte OutByte)
{       
  Serial1.write(OutByte);
} 
