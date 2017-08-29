void loop(){

  Check_Menu_Inst();

  switch(selected_mode){

    //================================================
  case PATTERN_MIDI_MASTER:
    // INIT---------------------------------------

    if( old_selected_mode!=PATTERN_MIDI_MASTER && !play ) {
      old_selected_mode=PATTERN_MIDI_MASTER;
      Disconnect_Callback();
      Mode_Synchro(0);//mode master synchro
      if(mute_mode)mute_mode=0;
      inst_mute=0;
      TapeTempoInit();
    }
    if (old_selected_mode==PATTERN_MIDI_SLAVE){
      MIDI.read();
      Check_Edit_Button_Pattern();
    }
    else if (old_selected_mode==PATTERN_DIN_SLAVE){
      Check_Edit_Button_Pattern();
    }
    else{
      Check_BPM();
      Check_Edit_Button_Pattern();
    }

    Check_Roll_Scale();
    Check_Edit_Button_Pattern();
    TestTapeTempo();   
    Mode_Pattern();
    Update_Pattern_EEprom();
    Update_Pattern_Led();
    break;

    //=================================================
  case PATTERN_DIN_SLAVE:
    // INIT----------------------------------------
    if((old_selected_mode!=PATTERN_DIN_SLAVE)&& !play){
      old_selected_mode=PATTERN_DIN_SLAVE;
      Disconnect_Callback();
      Mode_Synchro(1);//Din slave synchro
      if(mute_mode)mute_mode=0;
      inst_mute=0;
      //Serial.println("initilaize_PTR Din Slave");
    }
    //Serial.println (step_count,DEC); 
    if (old_selected_mode==PATTERN_MIDI_MASTER){
      Check_BPM();
      Check_Edit_Button_Pattern();
    }
    else if (old_selected_mode==PATTERN_MIDI_SLAVE){
      MIDI.read();
      Check_Edit_Button_Pattern();
    }
    else{
      Check_Edit_Button_Pattern();
    }
    Check_Roll_Scale();
    Mode_Pattern();
    Update_Pattern_EEprom();
    Update_Pattern_Led();
    break;

    //=================================================
  case PATTERN_MIDI_SLAVE:
    // INIT----------------------------------------
    if(old_selected_mode!=PATTERN_MIDI_SLAVE &&!play){
      old_selected_mode=PATTERN_MIDI_SLAVE;
      Disconnect_Callback();
      Mode_Synchro(2);//MIDI slave synchro
      if(mute_mode)mute_mode=0;
       MIDI.setHandleClock(Handle_Clock);// Callback Clock MIDI
      MIDI.setHandleStart(Handle_Start);// Callback Start MIDI
      MIDI.setHandleContinue(Handle_Start);// Callback Stop MIDI
      MIDI.setHandleStop(Handle_Stop);// Callback Stop MIDI
      inst_mute=0;
      //Serial.println("initilaize_PTR MIDI Slave");
    }
    if (old_selected_mode==PATTERN_MIDI_MASTER){
      Check_BPM();
      Check_Edit_Button_Pattern();
    }
    else if (old_selected_mode==PATTERN_DIN_SLAVE){
      Check_Edit_Button_Pattern();
    }
    else{
      MIDI.read();
      Check_Edit_Button_Pattern();
    }
    Check_Roll_Scale();
    Mode_Pattern();
    Update_Pattern_EEprom();
    Update_Pattern_Led();
    //todo
    break;

    //=================================================
  case PATTERN_EDIT:
    // INIT----------------------------------------
    if((old_selected_mode!=PATTERN_EDIT) &&!play){
      old_selected_mode=PATTERN_EDIT;
      Disconnect_Callback();
      Mode_Synchro(0);//mode master synchro
      if(mute_mode)mute_mode=0;
      /*if(play){
       play=0;
       MIDI_Send(0xfc);//envoi un stop midi
       PORTD &= ~(1<<5);//met au niveau bas la sorti DIN start =>STOP
       button_play_count=0;
       }*/
      inst_mute=0;
      roll_mode=0;
      //TapeTempoInit();
    }
    if(old_selected_mode==PATTERN_MIDI_MASTER || old_selected_mode==PATTERN_EDIT){

      Check_BPM();
    }
    else if (old_selected_mode==PATTERN_MIDI_SLAVE){
      MIDI.read();
    }
    Check_Edit_Button_Pattern_Edit();
    //    Timer1.initialize(timer_time); // set a timer of length in microseconds 
    //Serial.println(timer_time);
    //TestTapeTempo();
    Mode_Pattern();
    Update_Pattern_EEprom();
    Update_Pattern_Led();
    break;

    //=================================================
  case INIT_EEPROM:
    // INIT----------------------------------------
    if(old_selected_mode!=INIT_EEPROM){
      old_selected_mode=INIT_EEPROM;
      Disconnect_Callback();
      Mode_Synchro(2);//mode master synchro
      if(play){
        play=0;
        MIDI_Send(0xfc);//envoi un stop midi
        PORTD &= ~(1<<5);//met au niveau bas la sorti DIN start =>STOP
        button_play_count=0;
      }
    }

    Check_Edit_Button_Setup();
    Play_Version();
    Initialize_EEprom();//initialise les 256 patterns
    break;

    //==================================================
  case CLEAR_PATTERN:
    // INIT-----------------------------------------
    if(old_selected_mode!=CLEAR_PATTERN){
      old_selected_mode=CLEAR_PATTERN;
      Disconnect_Callback();
      Mode_Synchro(2);//mode master synchro
      first_time_clear_pattern=1;//flag que l'on vient de rentrée dans le mode clear pattern
      if(play){
        play=0;
        MIDI_Send(0xfc);//envoi un stop midi
        PORTD &= ~(1<<5);//met au niveau bas la sorti DIN start =>STOP
        button_play_count=0;
      }
    }

    Check_Edit_Button_Setup();
    if(!button_shift){
      SR.Led_Step_Write(1<<selected_pattern);
      PORTC&= ~(B11111100);//clear les edits leds dans ce mode 
    }
    else if(button_shift){
      SR.Led_Step_Write(1<<pattern_bank);
      PORTC |= B11111100;//clear les edits leds dans ce mode 
    }

    Clear_Pattern();
    break;


    //=================================================
  case COPY_PATTERN:
    // INIT----------------------------------------
    if(old_selected_mode!=COPY_PATTERN){
      old_selected_mode=COPY_PATTERN;
      Disconnect_Callback();
      Mode_Synchro(2);//mode master synchro
      first_time_copy_pattern=1;//flag que l'on vient de rentrée dans le mode copy pattern
      if(play){
        play=0;
        MIDI_Send(0xfc);//envoi un stop midi
        PORTD &= ~(1<<5);//met au niveau bas la sorti DIN start =>STOP
        button_play_count=0;
      }
    }

    Check_Edit_Button_Setup();
    if(!button_shift){
      SR.Led_Step_Write(1<<selected_pattern);
      PORTC&= ~(B11111100);//clear les edits leds dans ce mode 
    }
    else if(button_shift){
      SR.Led_Step_Write(1<<pattern_bank);
      PORTC |= B11111100;//clear les edits leds dans ce mode 
    }
    Copy_Pattern();
    break;

    //=================================================
  case PASTE_PATTERN:
    // INIT----------------------------------------
    if(old_selected_mode!=COPY_PATTERN){
      old_selected_mode=COPY_PATTERN;
      Disconnect_Callback();
      Mode_Synchro(2);//mode master synchro
      first_time_paste_pattern=1;//flag que l'on vient de rentrée dans le mode paste pattern
      if(play){
        play=0;
        MIDI_Send(0xfc);//envoi un stop midi
        PORTD &= ~(1<<5);//met au niveau bas la sorti DIN start =>STOP
        button_play_count=0;
      }
    }

    Check_Edit_Button_Setup();
    if(!button_shift){
      SR.Led_Step_Write(1<<selected_pattern);
      PORTC&= ~(B11111100);//clear les edits leds dans ce mode 
    }
    else if(button_shift){
      SR.Led_Step_Write(1<<pattern_bank);
      PORTC |= B11111100;//clear les edits leds dans ce mode 
    }
    Paste_Pattern();
    break;

    //================================================
  case SONG_EDIT:
    // INIT---------------------------------------
    if(old_selected_mode!=SONG_EDIT){
      old_selected_mode=SONG_EDIT;
      Disconnect_Callback();
      Mode_Synchro(0);//mode master synchro
      if(play){
        play=0;
        MIDI_Send(0xfc);//envoi un stop midi
        PORTD &= ~(1<<5);//met au niveau bas la sorti DIN start =>STOP
        button_play_count=0;
      }
      //TapeTempoInit();
    }

    Check_BPM();
    // Timer1.initialize(timer_time); // set a timer of length in microseconds 
    Check_Edit_Button_Song();
    //TestTapeTempo();
    Mode_Song_Edit();
    Update_Pattern_EEprom();
    Update_Song_EEprom();
    Update_Song_Led();
    break;

    //================================================
  case SONG_MIDI_MASTER:
    // INIT---------------------------------------
    if(old_selected_mode!=SONG_MIDI_MASTER){
      old_selected_mode=SONG_MIDI_MASTER;
      Disconnect_Callback();
      Mode_Synchro(0);//mode master synchro
      if(play){
        play=0;
        MIDI_Send(0xfc);//envoi un stop midi
        PORTD &= ~(1<<5);//met au niveau bas la sorti DIN start =>STOP
        button_play_count=0;
      }
      TapeTempoInit();
      
    }
    Check_BPM();
    Check_Edit_Button_Song();
    TestTapeTempo();
    Update_Song_EEprom();
    Update_Pattern_EEprom();
    Mode_Song_Play();
    Update_Song_Led();

    break;

    //================================================
  case SONG_DIN_SLAVE:
    // INIT---------------------------------------
    if(old_selected_mode!=SONG_DIN_SLAVE){
      old_selected_mode=SONG_DIN_SLAVE;
      Disconnect_Callback();
      Mode_Synchro(1);//mode master synchro
      if(play){
        play=0;
        MIDI_Send(0xfc);//envoi un stop midi
        PORTD &= ~(1<<5);//met au niveau bas la sorti DIN start =>STOP
        button_play_count=0;
      }
    }

    //Check_BPM();
    Check_Edit_Button_Song();
    Mode_Song_Play();
    Update_Pattern_EEprom();
    Update_Song_EEprom();
    Update_Song_Led();
    break;

    //================================================
  case SONG_MIDI_SLAVE:
    // INIT---------------------------------------
    if(old_selected_mode!=SONG_DIN_SLAVE){
      old_selected_mode=SONG_DIN_SLAVE;
      Disconnect_Callback();
      Mode_Synchro(2);//mode master synchro
      if(play){
        play=0;
        MIDI_Send(0xfc);//envoi un stop midi
        PORTD &= ~(1<<5);//met au niveau bas la sorti DIN start =>STOP
        button_play_count=0;
      }
      MIDI.setHandleClock(Handle_Clock);// Callback Clock MIDI
      MIDI.setHandleStart(Handle_Start);// Callback Start MIDI
      MIDI.setHandleContinue(Handle_Start);// Callback Stop MIDI
      MIDI.setHandleStop(Handle_Stop);// Callback Stop MIDI
    }

    //Check_BPM();
    MIDI.read();
    Check_Edit_Button_Song();
    Mode_Song_Play();
    Update_Pattern_EEprom();
    Update_Song_EEprom();
    Update_Song_Led();
    break;

    //================================================
  case MIDI_PLAY:
    // INIT---------------------------------------
    if(old_selected_mode!=MIDI_PLAY){
      old_selected_mode=MIDI_PLAY;
      PORTC&= ~(B11111100);//clear les edits leds dans ce mode  

      Disconnect_Callback();
      Mode_Synchro(2);

      MIDI.setHandleNoteOn(Handle_NoteOn);  // Callback NoteON
      MIDI.setHandleNoteOff(Handle_NoteOff);  // PCallback NoteOFF
      MIDI.setInputChannel(selected_channel+1);//initialise le channel midi
      if(play){
        play=0;
        MIDI_Send(0xfc);//envoi un stop midi
        PORTD &= ~(1<<5);//met au niveau bas la sorti DIN start =>STOP
        button_play_count=0;
      }
      //initialise les leds suivant le channel selectionner et les sortie des instru a 0
      Load_Midi_Channel();
      temp_step_led= (1<<selected_channel);
      MIDI.setInputChannel(selected_channel+1);
      SR.ShiftOut_Update(temp_step_led,0);
      for (int ct=0;ct<12;ct++) noteOnOff[ct]=0;
      Load_Midi_Note();
    } 

    MIDI.read();
    boolean indic = false;

    inst_midi_buffer=0;
    for (int ct=0;ct<16;ct++) 
    {
      if ( noteOnOff[ct]==1 ) 
      { 
        bitSet(inst_midi_buffer,ct);
        indic = true;
        noteOnOff[ct]=0;
      }
    }

    if (indic)
    {
      SR.ShiftOut_Update(temp_step_led,inst_midi_buffer);
     // Send_Trig_Out_Midi();
     //Serial.println(PINB,BIN);
      PORTB |= (1<<2);//envoie une impulsion sur la sorti trig CPU a chaque pas
      delayMicroseconds(10); 
      PORTB &= ~(1<<2);
      Send_Trig_Out_Midi();
    } 
    else
    {
      delayMicroseconds(1000); 
      SR.ShiftOut_Update(temp_step_led,0);
      Reset_Trig_Out();
    }  


    Check_Edit_Button_Setup();
    Check_Midi_Channel();
    if (selected_channel_changed){
      MIDI.setInputChannel(selected_channel+1);
      SR.ShiftOut_Update(temp_step_led,0);
      Save_Midi_Channel();
      selected_channel_changed=0;
    }
    Save_Midi_Note();//sauve le note midi si elles ont changé
    break;

  }
}









































