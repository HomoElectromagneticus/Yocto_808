void Mode_Pattern(){

  Verticalize_Pattern();// cf fonction, 


  unsigned int reading = SR.Button_Step_Read();// on lit les boutons
  if(reading!=old_step_button_state){// si ils ont changer on enregistre le temps
    millis_debounce_step_button=millis();
  }
  if((millis()-millis_debounce_step_button)>=DEBOUNCE){//apres le temps de debounce on compare la valeur des boutons avec l'ancienne valeur
    if(reading!=step_button_state){
      step_button_state=reading;

      //CHECK LE STATUT DES STEP BOUTONS-------------------------------------------------------------------
      for (byte i=0;i<16;i++){
        step_button_just_pressed[i]=0;//initialise l'array de la valeur de chaque bouton step
        step_button_current_state[i]= bitRead(reading,i);
        if (step_button_current_state[i]!=step_button_previous_state[i]){
          if ((step_button_pressed[i]== LOW) && (step_button_current_state[i] == HIGH)){
            step_button_just_pressed[i]=1;
          }
          step_button_pressed[i]=step_button_current_state[i];
        }
        step_button_previous_state[i]=step_button_current_state[i];
      }

      //Mode pattern Play
      //==============================================================================================================
      if(selected_mode==PATTERN_MIDI_MASTER || selected_mode==PATTERN_MIDI_SLAVE || selected_mode==PATTERN_DIN_SLAVE){

        //MUTE MODE----------------------------------------------------------------------------------------------------
        if(mute_mode){
          for (byte i=0;i<16;i++){
            if (step_button_just_pressed[i]){
              if(solo_mode){
                inst_mute = (B11111111 & (B11111111 <<8)) | ~(1<<i);//on mute tous les intrus sauf celui appuyer SOLO EXCLUSIF
                for (byte ct=0;ct<16;ct++){//on reinitialise les compteur des instrument smuter a 1 pour pouvoir les demuter en reappuyant dessus
                  step_button_count[ct]=1;
                }
                step_button_count[i]=0;
              }
              else step_button_count[i]++;
              switch (step_button_count[i]){
              case 1:            
                bitSet(inst_mute,i);     
                break;
              case 2:
                bitClear(inst_mute,i);
                step_button_count[i]=0;
                break;
              }
            }
          }
        }

        //SOLO MODE----------------------------------------------------------------------------------------------------
        /*  if(solo_mode){
         inst_mute = ~step_button_state;
         }*/
        //ROLL MODE----------------------------------------------------------------------------------------------------
        if(roll_mode){
          inst_roll=step_button_state;
          if((pattern_scale[pattern_buffer]%16)==0) scale_type=1;//permet de savoir si le roulement doit etre en ternaire ou binaire suivant la scale du pattern
          else scale_type=0;//0 = binaire scale   1= ternaire scale
        }
        //NORMAL MODE----------------------------------------------------------------------------------------------------
        else if (!mute_mode && !roll_mode){
          //Shift donc bank select
          if (button_shift){//si bouton shift appuyer
            for (byte i=0;i<16;i++){//loop autant de fois que de bouton step soit 16
              if (bitRead (step_button_state,i)){
                // si le bouton shift est appuyer on retourne la bank selectionner selectionner
                pattern_bank=i;
                selected_pattern_changed=1;//flag que le pattern selectionner a change
                break;
              }
            }
          }
          //Pas shift donc pattern select
          else{
            for (byte i=0;i<16;i++){//loop autant de fois que de bouton step soit 16
              if (step_button_just_pressed[i]){//si un bouton est papuyer
                pushed_button_step_count++;//incrementer le compteur du nombre de bouton appuyer
                if(pushed_button_step_count==1){//si c'est le premier bouton appuyer
                  first_selected_pattern=i;//on enregistre comme le premier pattern selectionner
                  selected_pattern_changed=1;//flag que le pattern selectionner a change
                  last_selected_pattern=255; //on initialise le dernier pattern selectionne         
                }
                else if (pushed_button_step_count==2){//c'est le deuxieme bouton appuyer
                  last_selected_pattern=i;//on enregistre comme le dernier pattern selectionner
                  pushed_button_step_count=0;//on initialise le compteur du nombre de bouton appuyer
                  break;
                }    
              }
            }
            //Cette fonction permet que le variable selected_pattern stock toujours le plus petit pattern selectionner
            if ((first_selected_pattern > last_selected_pattern)&& (last_selected_pattern!=255)){//si le premier pattern selectionner est plus grand que le dernier alors le pattern selectionner est egal au dernier pattern
              low_selected_pattern=last_selected_pattern;
              selected_pattern_changed=1;//flag que le pattern selectionner a change
            }
            else{//sinon il est egal au premier
              low_selected_pattern=first_selected_pattern;
              selected_pattern_changed=1;//flag que le pattern selectionner a change
            }
            selected_pattern=low_selected_pattern;

            if (last_selected_pattern==255){//si il n'y a pas de dernier pattern selectionner 
              nbr_pattern_block=0;//0 pattern dans le block
            }
            else{//sinon
              nbr_pattern_block = abs(last_selected_pattern-first_selected_pattern);//le difference du premier et du dernier pattern donne le nombre de pattern dans le block
              nbr_pattern_block_changed=1;//flag que le block a change sert pour reinitialiser en stop
              if(play){
                nbr_pattern_block_changed_A=1;//flag que le block a change sert dans l'interuption
              }
            }
          }
          //pattern_nbr = selected_pattern+(16*pattern_bank);//le numero du pattern est egal au pattern selectionner plus 16 fois la bank soit 
          //255 pattern

        }
      }

      //Mode Pattern Edit
      //==============================================================================================================
      if(selected_mode==PATTERN_EDIT){
        if(play){//si seq en run 
          if (button_shift){//si bouton shift appuyer
            for (byte i=0;i<16;i++){//loop autant de fois que de bouton step soit 16
              if (bitRead (step_button_state,i)){
                // si le bouton shift est appuyer on retourne la valeur du nombre de step
                nbr_step[pattern_buffer]=(i+(16*button_pattern_part))+1;
                nbr_step_changed=1;//le nombre de pas du pattern a change
              }
            }
          }
          else{// sinon bouton shift pas appuyer on retourne la valeur des boutons dans dans le pattern editer
            selected_pattern_edited=1;//flag que le pattern a ete editer depuis la derniere sauvegarde
            selected_pattern_edited_saved=1;//flag que le pattern a ete editer depuis la derniere sauvegarde
            //pattern_in_midi_buffer=0;
            pattern[pattern_buffer][selected_inst][button_pattern_part] ^= step_button_state ;//le pattern egale la valeur des boutons appuyer 
            //Serial.println(pattern[pattern_buffer][selected_inst][button_pattern_part],BIN);
          }
        }
        else if (!play){//si pas en play
          if (button_shift){//si bouton shift appuyer
            for (byte i=0;i<16;i++){//loop autant de fois que de bouton step soit 16
              if (bitRead (step_button_state,i)){
                // si le bouton shift est appuyer on retourne la bank selectionner selectionner
                pattern_bank=i;
                break;
              }
            }
          }
          else{
            for (byte i=0;i<16;i++){//loop autant de fois que de bouton step soit 16
              if (bitRead (step_button_state,i)){
                //on retourne la valeur du pattern selectionner
                selected_pattern=i;
                break;
              }
            }
          }
          //pattern_nbr = selected_pattern+(16*pattern_bank);//le numero du pattern est egal au pattern selectionner plus 16 fois la bank soit 
          //255 pattern
          //Serial.println(pattern_nbr,DEC);
        }
      }
    }
    else if (step_button_state==0){//si on a relacher tous les boutons 
      pushed_button_step_count=0;//on initialise le compteur du nombre de bouton appuyer
    }
  }

  //BLOCK SELECTIONNER ------------------------------------
  //Fonction qui fait avancer le pattern selectionner quand un block est selectionner
  if ((nbr_pattern_block!=0)&& (middle_mesure_flag)){//un block est se'lectionner et on a avnacer dans la mesure 
    selected_pattern=low_selected_pattern + pattern_count;//le pattern selectionner egal le premier pattern + le coount pattern qui incrementer dans l'interuption
    selected_pattern_changed=1;//flag que le pattern selectionner a change permet de verticalize
    middle_mesure_flag=0;//reset flag que la mesure a avancé
  }
  if(!play){
    nbr_pattern_block_changed_A=0;//flag que le block a change
    if(nbr_pattern_block_changed){
      nbr_pattern_block_changed=0;
      middle_mesure_flag=0;
      selected_pattern=low_selected_pattern;
      selected_pattern_changed=1;//flag que le pattern selectionner a change
    }
  }
  old_step_button_state = reading;
  pattern_nbr = selected_pattern+(16*pattern_bank);//le numero du pattern est egal au pattern selectionner plus 16 fois la bank soit 255 pattern

  //====================================================================
  //fonction clear avec le bouton de l'encoder
  //clear le pas en train etre jouer quand on appuie dessus
  if (button_encoder && selected_mode==PATTERN_EDIT){
    if(step_count<16){
      bitClear(pattern[pattern_buffer][selected_inst][0], step_count);
    }
    else if(step_count>=16){
      bitClear(pattern[pattern_buffer][selected_inst][1], step_count-16);
    }
    selected_pattern_edited=1;//flag que le pattern a ete editer depuis la derniere sauvegarde
    selected_pattern_edited_saved=1;//flag que le pattern a ete editer depuis la derniere sauvegarde
  }
  //Unmute all
  else if(button_encoder&& (selected_mode==PATTERN_MIDI_MASTER || selected_mode==PATTERN_MIDI_SLAVE || selected_mode==PATTERN_DIN_SLAVE)&& mute_mode){
    inst_mute=0;
    for (byte i=0;i<16;i++){
      step_button_count[i]=0;

    }
  }
}















































