void Check_Edit_Button_Pattern_Edit()
{

  byte reading= SR.Button_SR_Read(2);//stock l'etat des boutons
  if (reading!=old_edit_button_state){
    millis_debounce_edit_button=millis();
    //Serial.println("ok");
  }
  if((millis()-millis_debounce_edit_button)>=DEBOUNCE){
    if(reading!=edit_button_state){
      edit_button_state=reading;
      switch (edit_button_state){
        //----------------------
        //check le bouton play
      case 1:
        if(old_selected_mode==0 || old_selected_mode==3){
          button_play_count++;
          if(button_play_count==1){
            play=1;
            first_play=1;//permet de jouer le premier pas (cf interrupt)
            first_play_A=1;//un deuxime flag de jouer le premier (cf interrupt)
          }
          else if (button_play_count==2){
            play=0;
            button_play_count=0;
            first_stop=1;
          }
        }
        //Serial.println(play,DEC);//DEBUGG
        break;
        //-------------------------
        //check bouton scale
      case 4:
        pattern_scale_changed=1;//indique que la scale du pattern achanger
        button_scale_count++;
        if(button_scale_count==4) button_scale_count=0;
        //envoi la scale correspondante dans la variable pattern scale
        switch (button_scale_count){
        case 0:
          pattern_scale[pattern_buffer]=12;// 1/32 de temps
          break;
        case 1:
          pattern_scale[pattern_buffer]=24;// 1/16 de temps
          break;
        case 2:
          pattern_scale[pattern_buffer]=16;// 1/16t de temps
          break;
        case 3:
          pattern_scale[pattern_buffer]=32;// 1/8t de temsp
          break;
        }
        //Serial.println(button_scale_count,DEC);//DEBUGG
        break;
        //-------------------------
        //check quel part est selectionner soit 1a16 ou 17a32
      case 8:
        button_pattern_part=1;//section 17 a 32 selectionner
        break;
      case 16:
        button_pattern_part=0;// section 1 a 16 selectionner
        break;
      }
      //les deux boutons 1-16 et 17-32 appuyer
      if (edit_button_state==24){
        button_part_switch =!button_part_switch;
      }
      //Bouton Shift 
      if (edit_button_state==2) button_shift=1;//bouton shift appuyer
      else button_shift=0;//bouton shift relacher
      if (edit_button_state==32) button_encoder=1;
      else button_encoder=0;
      // Serial.println(button_encoder);//DEBUGG
      //Serial.println(button_init,DEC);//DEBUGG
    }
  }
  old_edit_button_state= reading;// on retient l'etat des boutons
}

//=============================================================================
void Check_Edit_Button_Pattern()
{
  //-----------------------------------------------------
  if (sync_mode != MASTER)
  {
    //Check la statut play en mdoe DIN_SYNC
    boolean din_start_state = PIND & (1<<5);
    if (din_start_state != old_din_start_state){
      if(din_start_state){//si l'entrée DIN Start est HIGH
        first_play=1;// indiquer le premier play
        play=1;

      }
      else if (!din_start_state){//si l'entrée DIN Start est LOW
        play=0;
        first_stop=1;
      }
      old_din_start_state = din_start_state;
    }
  }

  //-------------------------------------------------------
  byte reading= SR.Button_SR_Read(2);//stock l'etat des boutons
  if (reading!=old_edit_button_state){
    millis_debounce_edit_button=millis();
    //Serial.println("ok");
  }
  if((millis()-millis_debounce_edit_button)>=DEBOUNCE){
    if(reading!=edit_button_state){
      edit_button_state=reading;
      switch (edit_button_state){
        //----------------------
        //check le bouton play
      case 1:
        if(sync_mode == MASTER){//selected_mode==0 || selected_mode==3){
          button_play_count++;
          if(button_play_count==1){
            //ppqn_count=0;
            play=1;
            first_play=1;//permet de jouer le premier pas (cf interrupt)
            first_play_A=1;//un deuxime flag de jouer le premier (cf interrupt)
            first_play_C=1;//idem
          }
          else if (button_play_count==2){
            play=0;
            button_play_count=0;
            first_stop=1;
          }
        }
        //Serial.println(play,DEC);//DEBUGG
        break;
        //-------------------------
        //check bouton scale
      case 4:
        break;
        //-------------------------
        //check quel bouton de la part est selectionner soit 1a16 ou 17a3
      case 8:
        roll_mode=1;//indique qu'on est rentré dans le mode roll
        mute_mode=0;
        break;
      case 16:
        mute_mode=1;//indique qu'on est dans le mute mode
        roll_mode=0;
        break;
      }
      if((mute_mode==1||roll_mode==1) && button_shift==1) {
        mute_mode=0;//on sort du mode mode avec le bouton shift 
        roll_mode=0;
      }
      //-------------------------
      //Bouton Shift 
      if (edit_button_state==2) button_shift=1;//bouton shift appuyer
      else button_shift=0;//bouton shift relacher
      //Bouton encoder
      if (edit_button_state==32) button_encoder=1;
      else button_encoder=0;
      //Bouton Solo mode
      if (edit_button_state==16 && mute_mode==1) solo_mode=1;
      else solo_mode=0;
    }
  }
  old_edit_button_state= reading;// on retient l'etat des boutons
}

//==================================================================
void Check_Edit_Button_Setup()
{
  byte reading= SR.Button_SR_Read(2);//stock l'etat des boutons
  if (reading!=old_edit_button_state){
    millis_debounce_edit_button=millis();
    //Serial.println("ok");
  }
  if((millis()-millis_debounce_edit_button)>=DEBOUNCE){
    if(reading!=edit_button_state){
      edit_button_state=reading;
      //-------------------------
      //Bouton play
      if (edit_button_state==1) button_play=1;//bouton play appuyer
      else button_play=0;//bouton play relacher
      //-------------------------
      //Bouton Shift 
      if (edit_button_state==2) button_shift=1;//bouton shift appuyer
      else button_shift=0;//bouton shift relacher
      //-------------------------
      //Check si les boutons play et shift sont apuyer
      if(edit_button_state== 3) button_init=1;// bouton init correspond au bouton shift et play appuyer
      else button_init=0;
      //--------------------
      //Bouton reset 
      if (edit_button_state==16) button_reset=1;//bouton shift appuyer
      else button_reset=0;//bouton shift relacher

    }
  }
  old_edit_button_state= reading;// on retient l'etat des boutons
}

//==================================================================
void Check_Edit_Button_Song()
{
  //-------------------------------------------------------------------------------------
  if (selected_mode ==5 || selected_mode ==6){//le mode selectionne est il SONG_DIN_SLAVe ou SONG_MIDI_SLAVE
    //Check la statut play en mdoe DIN_SYNC
    boolean din_start_state = PIND & (1<<5);
    if (din_start_state != old_din_start_state){
      if(din_start_state){//si l'entrée DIN Start est HIGH
        first_play=1;// indiquer le premier play
        first_play_A=1;//un deuxime flag de jouer le premier (cf interrupt)
        first_play_B=1;
        play=1;

      }
      else if (!din_start_state){//si l'entrée DIN Start est LOW
        play=0;
        first_stop=1;
      }
      old_din_start_state = din_start_state;
    }
  }
  byte reading= SR.Button_SR_Read(2);//stock l'etat des boutons
  if (reading!=old_edit_button_state){
    millis_debounce_edit_button=millis();
  }
  if((millis()-millis_debounce_edit_button)>=DEBOUNCE){
    if(reading!=edit_button_state){
      edit_button_state=reading;
      //---------------------------------------------------------------------------------
      if (selected_mode== 4 || selected_mode==7){//le mode selectionne est il SONG_MIDI_MASTER
        switch (edit_button_state){
          //----------------------
          //check le bouton play
        case 1:   
          button_play_count++;
          if(button_play_count==1){
            play=1;
            first_play=1;//permet de jouer le premier pas (cf interrupt)
            first_play_A=1;//un deuxime flag de jouer le premier (cf interrupt)
            first_play_B=1;
          }
          else if (button_play_count==2){
            play=0;
            button_play_count=0;
            first_stop=1;
          } 
          break;
        }
      }

      //--------------------
      //Bouton Shift 
      if (edit_button_state==2) button_shift=1;//bouton shift appuyer
      else button_shift=0;//bouton shift relacher
      //--------------------
      //Bouton Next 
      if (edit_button_state==4){
        button_next=1;//bouton shift appuyer
      }
      else{
        button_next=0;//bouton shift relacher
        first_push_next=0;//flag que le bouton next a été relacher
      }
      //--------------------
      //Bouton End 
      if (edit_button_state==8){
        button_end=1;//bouton shift appuyer
      }
      else {
        button_end=0;//bouton shift relacher
        first_push_end=0;//flag que le bouton end a ete relacher
      }
      //--------------------
      //Bouton reset 
      if (edit_button_state==16) button_reset=1;//bouton shift appuyer
      else button_reset=0;//bouton shift relacher
      //Bouton encoder
      if (edit_button_state==32) button_encoder=1;
      else button_encoder=0;

    }
  }
  old_edit_button_state= reading;// on retient l'etat des boutons
  //Serial.println(button_reset);
}

































