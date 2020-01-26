void loop()
{

    Check_Menu_Inst();

    switch (selected_mode) {

    //================================================
    case PATTERN_MIDI_MASTER:
    case PATTERN_DIN_SLAVE:
    case PATTERN_MIDI_SLAVE:

        // INIT---------------------------------------
        if (!play && old_selected_mode != selected_mode) {
            old_selected_mode = selected_mode;
            Disconnect_Callback();

            switch (selected_mode) {
            case PATTERN_MIDI_MASTER:
                Mode_Synchro(0); //mode master synchro
                TapeTempoInit();
                break;

            case PATTERN_DIN_SLAVE:
                Mode_Synchro(1); //Din slave synchro
                break;

            case PATTERN_MIDI_SLAVE:
                Mode_Synchro(2); //MIDI slave synchro
                MIDI.setHandleClock(Handle_Clock);    // Callback Clock MIDI
                MIDI.setHandleStart(Handle_Start);    // Callback Start MIDI
                MIDI.setHandleContinue(Handle_Start); // Callback Continue MIDI
                MIDI.setHandleStop(Handle_Stop);      // Callback Stop MIDI
                break;
            }

            if (mute_mode) {
                mute_mode = 0;
            }

            inst_mute = 0;
        }

        if (first_play) {
            sync_fallback = sync_mode;
        }

        if (sync_mode == MASTER) {
            Check_BPM();
            TestTapeTempo();
        }
        else if (sync_mode == MIDI_SLAVE) {
            MIDI.read();
        }

        Check_Edit_Button_Pattern();
        Check_Roll_Scale();
        Mode_Pattern();
        Update_Pattern_EEprom();
        Update_Pattern_Led();
        break;

    //=================================================
    case PATTERN_EDIT:

        // INIT----------------------------------------
        if (old_selected_mode != PATTERN_EDIT) {
            old_selected_mode = PATTERN_EDIT;

            if (!play) {
                Mode_Synchro(sync_fallback);
            }
        }

        if (sync_mode == MASTER) {
            Check_BPM();
        }
        else if (sync_mode == MIDI_SLAVE) {
            MIDI.read();
        }

        Check_Edit_Button_Pattern_Edit();
        Mode_Pattern();
        Update_Pattern_EEprom();
        Update_Pattern_Led();
        break;

    //=================================================
    case INIT_EEPROM:

        // INIT----------------------------------------
        if (old_selected_mode != INIT_EEPROM) {
            old_selected_mode = INIT_EEPROM;
            Disconnect_Callback();
            Mode_Synchro(2);//mode master synchro

            if (play) {
                play = 0;
                MIDI_Send(0xfc);//envoi un stop midi (send a MIDI stop)
                Set_Dinsync_Run_Low();
                button_play_count = 0;
            }
        }

        Check_Edit_Button_Setup();
        Play_Version();
        Initialize_EEprom();//initialise les 256 patterns (init. the 256 patterns)
        break;

    //==================================================
    case CLEAR_PATTERN:

        // INIT-----------------------------------------
        if (old_selected_mode != CLEAR_PATTERN) {
            old_selected_mode = CLEAR_PATTERN;
            Disconnect_Callback();
            Mode_Synchro(2);//mode master synchro
            // flag that we have just reentered the "clear pattern" mode
            first_time_clear_pattern = 1; //flag que l'on vient de rentrée dans le mode clear pattern

            if (play) {
                play = 0;
                MIDI_Send(0xfc);//envoi un stop midi (send a MIDI stop)
                Set_Dinsync_Run_Low();
                button_play_count = 0;
            }
        }

        Check_Edit_Button_Setup();

        if (!button_shift) {
            SR.Led_Step_Write(1 << selected_pattern);
            PORTC &= ~(B11111100);//clear les edits leds dans ce mode
        }
        else if (button_shift) {
            SR.Led_Step_Write(1 << pattern_bank);
            PORTC |= B11111100;//clear les edits leds dans ce mode
        }

        Clear_Pattern();
        break;


    //=================================================
    case COPY_PATTERN:

        // INIT----------------------------------------
        if (old_selected_mode != COPY_PATTERN) {
            old_selected_mode = COPY_PATTERN;
            Disconnect_Callback();
            Mode_Synchro(2);//mode master synchro
            first_time_copy_pattern = 1; //flag que l'on vient de rentrée dans le mode copy pattern

            if (play) {
                play = 0;
                MIDI_Send(0xfc);//envoi un stop midi (send a MIDI stop)
                Set_Dinsync_Run_Low();
                button_play_count = 0;
            }
        }

        Check_Edit_Button_Setup();

        if (!button_shift) {
            SR.Led_Step_Write(1 << selected_pattern);
            PORTC &= ~(B11111100);//clear les edits leds dans ce mode
        }
        else if (button_shift) {
            SR.Led_Step_Write(1 << pattern_bank);
            PORTC |= B11111100;//clear les edits leds dans ce mode
        }

        Copy_Pattern();
        break;

    //=================================================
    case PASTE_PATTERN:

        // INIT----------------------------------------
        if (old_selected_mode != COPY_PATTERN) {
            old_selected_mode = COPY_PATTERN;
            Disconnect_Callback();
            Mode_Synchro(2);//mode master synchro
            first_time_paste_pattern = 1; //flag que l'on vient de rentrée dans le mode paste pattern

            if (play) {
                play = 0;
                MIDI_Send(0xfc);//envoi un stop midi
                Set_Dinsync_Run_Low();
                button_play_count = 0;
            }
        }

        Check_Edit_Button_Setup();

        if (!button_shift) {
            SR.Led_Step_Write(1 << selected_pattern);
            PORTC &= ~(B11111100);//clear les edits leds dans ce mode
        }
        else if (button_shift) {
            SR.Led_Step_Write(1 << pattern_bank);
            PORTC |= B11111100;//clear les edits leds dans ce mode
        }

        Paste_Pattern();
        break;

    //================================================
    //================================================
    case SONG_MIDI_MASTER:
    case SONG_DIN_SLAVE:
    case SONG_MIDI_SLAVE:

        // INIT---------------------------------------
        if (!play && old_selected_mode != selected_mode) {
            old_selected_mode = selected_mode;
            Disconnect_Callback();

            switch (selected_mode) {
            case SONG_MIDI_MASTER:
                Mode_Synchro(0); //mode master synchro
                TapeTempoInit();
                break;

            case SONG_DIN_SLAVE:
                Mode_Synchro(1); //Din slave synchro
                break;

            case SONG_MIDI_SLAVE:
                Mode_Synchro(2); //MIDI slave synchro
                MIDI.setHandleClock(Handle_Clock);    // Callback Clock MIDI
                MIDI.setHandleStart(Handle_Start);    // Callback Start MIDI
                MIDI.setHandleContinue(Handle_Start); // Callback Continue MIDI
                MIDI.setHandleStop(Handle_Stop);      // Callback Stop MIDI
                break;
            }

            if (mute_mode) {
                mute_mode = 0;
            }

            inst_mute = 0;
        }

        if (first_play) {
            sync_fallback = sync_mode;
        }

        if (sync_mode == MASTER) {
            Check_BPM();
            TestTapeTempo();
        }
        else if (sync_mode == MIDI_SLAVE) {
            MIDI.read();
        }

        Check_Edit_Button_Song();
        Mode_Song_Play();
        Update_Pattern_EEprom();
        Update_Song_EEprom();
        Update_Pattern_Led();
        Update_Song_Led();
        break;

    //================================================
    case SONG_EDIT:

        // INIT----------------------------------------
        if (old_selected_mode != SONG_EDIT) {
            old_selected_mode = SONG_EDIT;

            if (!play) {
                Mode_Synchro(sync_fallback);
            }
        }

        if (sync_mode == MASTER) {
            Check_BPM();
        }
        else if (sync_mode == MIDI_SLAVE) {
            MIDI.read();
        }

        Check_Edit_Button_Song();
        Mode_Song_Edit();
        Update_Pattern_EEprom();
        Update_Song_EEprom();
        Update_Song_Led();
        break;

    //================================================
    case MIDI_PLAY:

        // INIT---------------------------------------
        if (old_selected_mode != MIDI_PLAY) {
            old_selected_mode = MIDI_PLAY;
            PORTC &= ~(B11111100);//clear les edits leds dans ce mode

            Disconnect_Callback();
            Mode_Synchro(2);

            MIDI.setHandleNoteOn(Handle_NoteOn);  // Callback NoteON
            MIDI.setInputChannel(selected_channel + 1); //initialise le channel midi

            if (play) {
                play = 0;
                MIDI_Send(0xfc);//envoi un stop midi
                Set_Dinsync_Run_Low();
                button_play_count = 0;
            }

            //initialise les leds suivant le channel selectionner et les sortie des instru a 0
            Load_Midi_Channel();
            temp_step_led = (1 << selected_channel);
            MIDI.setInputChannel(selected_channel + 1);
            SR.ShiftOut_Update(temp_step_led, 0);

            for (int ct = 0; ct < 12; ct++) {
                noteOnOff[ct] = 0;
            }

            Load_Midi_Note();
        }

        // Check user interface
        Check_Edit_Button_Setup();
        Check_Midi_Channel();

        if (selected_channel_changed) {
            MIDI.setInputChannel(selected_channel + 1);
            SR.ShiftOut_Update(temp_step_led, 0);
            Save_Midi_Channel();
            selected_channel_changed = 0;
        }

        Save_Midi_Note();//sauve le note midi si elles ont changé

        if (midi_led_flash_count > 0) {
            --midi_led_flash_count;

            if (midi_led_flash_count == 0) {
                PORTC &= ~MIDI_ACTIVITY_LED;
            }
        }

        if (midi_trig_pulse_count > 0) {
            --midi_trig_pulse_count;

            if (midi_trig_pulse_count == 0) {
                Reset_Trig_Out();
                SR.ShiftOut_Update(temp_step_led, 0);
            }
        }

        // Check for MIDI data
        inst_midi_buffer = 0;
        MIDI.read();

        if (inst_midi_buffer == 0) {
            // Nothing we can do with the MIDI data, go to next loop.
            return;
        }

        // The order of what follows is important.
        //
        // 1. Set pins HIGH for the instrument(s) we want to trigger.
        // 2. Send a 10 microsecond pulse to the TRIG_CPU input of the Accent circuit
        // 3. The TRIG_CPU pulse triggers a monostable  in the Accent circuit which creates a 1
        //    millisecond (inverse) pulse.
        // 4. After the 1 ms Accent pulse is done, return the instrument trigger pins to LOW.
        //
        // It is OK if step 4 is 'late' but if it is too short it affects the
        // envelopes of the instruments. The instrument trigger must remain high
        // until the monostable pulse inside the Accent circuit is done.
        //
        SR.ShiftOut_Update(temp_step_led, inst_midi_buffer);

        Set_CPU_Trig_High();
        delayMicroseconds(10);
        Set_CPU_Trig_Low();

        Send_Trig_Out_Midi();

        // This number is tuned to the speed of the main loop. It needs to be
        // more than 1ms to give Accent monostable enough time to create its 1ms
        // pulse.
        midi_trig_pulse_count = 7;


        break;

    case EEPROM_DUMP:
        Check_Edit_Button_Setup();

        if (old_selected_mode != EEPROM_DUMP) {
            old_selected_mode = EEPROM_DUMP;
            //Serial.println("EEPROM_DUMP");
        }

        if (button_play) {
            Dump_EEprom();
        }

        break;

    case EEPROM_RECEIVE:
        Check_Edit_Button_Setup();

        if (old_selected_mode != EEPROM_RECEIVE) {
            old_selected_mode = EEPROM_RECEIVE;
            //Serial.println("EEPROM_RECEIVE");
        }

        while (MIDI.read()) {
            if (MIDI.getType() >= 0xf0) { // SysEX
                Receive_EEprom(MIDI.getSysExArray(), MIDI.getSysExArrayLength());
            }
        }

        SR.Led_Step_Write(0); // Disable the LEDs.
        break;

    }

}
