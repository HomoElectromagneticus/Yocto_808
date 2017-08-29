//=======================================================================
//fonction qui initialize un pattern vide avec 16pas et la scale en 1/16
//=======================================================================

void Clear_Pattern(){

  Check_Edit_Button_Setup();//check si les boutons edit en mode setup

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
            selected_pattern=i;
            selected_pattern_changed=1;//flag que le pattern selectionner a change
            break;
          }
        }
      }
      else if (button_shift){
        for (byte i=0;i<16;i++){//loop autant de fois que de bouton step soit 16
          if (bitRead (step_button_state,i)){
            // si le bouton shift est appuyer on retourne la bank selectionner selectionner
            pattern_bank=i;
            selected_pattern_changed=1;//flag que le pattern selectionner a change
            break;
          }
        }
      }
      pattern_nbr = selected_pattern+(16*pattern_bank);//le numero du pattern est egal au pattern selectionner plus 16 fois la bank soit 255 pattern
    }
  }
  old_step_button_state=reading;

  // si le pattern selectionner a changer
  if(old_pattern_nbr != pattern_nbr)
  {
    Load_Pattern();
    pattern_clear_ok=0;//le pattern selectionner n'a pas été clearer
  }

  //Si on appuie sur play et que le pattern n'a pas necore etait clearer ou a ete editer ou qu'on vient de rentrer dans le mode Clear_Pattern
  if(  button_play && ( !pattern_clear_ok || selected_pattern_changed || first_time_clear_pattern ) ) 
  { 
    first_time_clear_pattern=0;//initialise le flag d'entrée dans le mode pattern clear
    selected_pattern_changed=0;
    Clear_EEprom_Pattern();
    Chenillard();
    pattern_clear_ok=1;//le pattern selectionner a ete clearer
    pattern_buffer=!pattern_buffer;
    Load_Pattern();//on recharge le pattern clearer
    //pattern_buffer=!pattern_buffer;
    selected_pattern_changed=0;//le pattern selectionner est reseter
  }   
}

//=======================================================================
//fonction qui copie le pattern selectionner dans le buffer
//=======================================================================

void Copy_Pattern()
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
            selected_pattern=i;
            selected_pattern_changed=1;//flag que le pattern selectionner a change
            break;
          }
        }
      }
      else if (button_shift){
        for (byte i=0;i<16;i++){//loop autant de fois que de bouton step soit 16
          if (bitRead (step_button_state,i)){
            // si le bouton shift est appuyer on retourne la bank selectionner selectionner
            pattern_bank=i;
            selected_pattern_changed=1;//flag que le pattern selectionner a change
            break;
          }
        }
      }
      pattern_nbr = selected_pattern+(16*pattern_bank);//le numero du pattern est egal au pattern selectionner plus 16 fois la bank soit 255 pattern
    }
  }
  old_step_button_state=reading;



  // si le pattern selectionner a changer
  if(old_pattern_nbr != pattern_nbr) {
    Load_Pattern();
    pattern_copy_ok=0;//le pattern selectionner n'a pas été copier
  }

  if( button_play && (!pattern_copy_ok || selected_pattern_changed || first_time_copy_pattern ) ) {

    first_time_copy_pattern=0;//initialise le flag d'entrée dans le mode pattern clear
    selected_pattern_changed=0;
    for (char i =0;i<NBR_INST;i++){//loop autant de fois que d'instruments
      for (char j=0;j<2;j++){//loop deux fois pour les deux parti du pattern 1a16 et 17 a 32
        // copy_pattern_buffer[i][j] =0;//initialise le buffer 
        copy_pattern_buffer[i][j] = pattern[pattern_buffer][i][j];//On met le pattern a copier dans le buffer
      }
    }
    copy_pattern_nbr_step=nbr_step[pattern_buffer];//copie le nombre de pas dans le buffer
    copy_pattern_scale=pattern_scale[pattern_buffer];//copie la scale dans le buffer

    //fonction chenillard des leds qui indique que le pattern a ete copier
    Chenillard();
    pattern_copy_ok=1;//le pattern selectionner a ete copier
    selected_pattern_changed=0;//initialise que le pattern changer a été copier  
  }
}

//=======================================================================
//fonction qui paste le pattern dans le buffer dans le pattern selectionne
//=======================================================================

void Paste_Pattern()
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
            selected_pattern=i;
            selected_pattern_changed=1;//flag que le pattern selectionner a change
            break;
          }
        }
      }
      else if (button_shift){
        for (byte i=0;i<16;i++){//loop autant de fois que de bouton step soit 16
          if (bitRead (step_button_state,i)){
            // si le bouton shift est appuyer on retourne la bank selectionner selectionner
            pattern_bank=i;
            selected_pattern_changed=1;//flag que le pattern selectionner a change
            break;
          }
        }
      }
      pattern_nbr = selected_pattern+(16*pattern_bank);//le numero du pattern est egal au pattern selectionner plus 16 fois la bank soit 255 pattern
    }
  }
  old_step_button_state=reading;

  if( button_play && ( selected_pattern_changed || first_time_paste_pattern ) )
  {
    selected_pattern_changed=false;
    first_time_paste_pattern=0;//initialise le flag d'entrée dans le mode pattern clear
    Save_Paste_Pattern();
    Chenillard();
  }
}












