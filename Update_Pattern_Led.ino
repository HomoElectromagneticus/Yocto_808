void Update_Pattern_Led()
{
  //Mode pattern Play
  //-----------------------------------------
  if(selected_mode==PATTERN_MIDI_MASTER || selected_mode==PATTERN_MIDI_SLAVE || selected_mode==PATTERN_DIN_SLAVE){



    if(play&&mute_mode){
      temp_step_led= (inst_step_buffer[step_count][pattern_buffer]*tempo_led_flag) | inst_mute;
      PORTC = (1*!tempo_led_flag)<<7;//fait clignoter la led de mute mode 
    }
    else if (play&&roll_mode){
      temp_step_led= (inst_step_buffer[step_count][pattern_buffer]*tempo_led_flag) | inst_roll;
      PORTC = (1*!tempo_led_flag)<<6;//fait clignoter la led de roll mode     
    }
    else if(!play&&mute_mode){     
      temp_step_led= inst_mute;
      PORTC = (1*!tempo_led_flag)<<7;//fait clignoter la led de mute mode 
    }
    else if (!play&&roll_mode){
      temp_step_led= inst_roll;
      PORTC = (1*!tempo_led_flag)<<6;//fait clignoter la led de mute mode
    }
    else if (!mute_mode){

      unsigned int temp_led_block=0;//variable qui sert a calcul quel led seront allumer suivant le block selectionner
      unsigned int temp_led_pattern=0;//idem

      //-------------------------------------------------------------------
      //fonction qui calcul quelles leds sont allumer entre les deux pattern selectionner
      if(first_selected_pattern<last_selected_pattern){//1e cas le premier pattern est plus petit que le dernier
        temp_led_block = (((1<<last_selected_pattern)-1) | (1<<last_selected_pattern));
        temp_led_pattern = ((1<<first_selected_pattern)-1);
      }
      else if (first_selected_pattern>last_selected_pattern){//2e cas le premier patern est plsu grand que le dernier
        temp_led_block = ((1<<last_selected_pattern)-1);
        temp_led_pattern = (((1<<first_selected_pattern)-1) | (1<<first_selected_pattern));
      }
      //PAS PLAY et PAS SHIFT----------------------------------------------
      if(!play && !button_shift){
        if (last_selected_pattern==255){//s'il n'y a pas de block, un seul pattern selectionner
          temp_step_led=((1*tempo_led_flag<<selected_pattern));// | (selected_pattern_led));//si pas play et pas shift button la led du pattern selectionner clignote au tempo
        }
        else{
          temp_step_led=((1*tempo_led_flag<<selected_pattern)^(temp_led_block ^ temp_led_pattern));
        }

        PORTC = (1<<button_scale_count+2) + (1<<!button_pattern_part+6);//Update les led de la parti edit suivant la scale et le nombre de pas auquel le sequenceur en est
      }
      //PAS PLAY ET SHIFT---------------------------------------------------
      else if(!play && button_shift){
        temp_step_led=(1<<pattern_bank);//si pas play et shift button le led indique la bank selectionner
        PORTC=B11111100;//allumer toutes leds pour indiquer qu'on selectionne la bank
      }
      //PLAY ET PAS SHIFT---------------------------------------------------
      else if (play && !button_shift){
        if (last_selected_pattern==255){//s'il n'y a pas de block, un seul pattern selectionner
          temp_step_led=(1<<selected_pattern ^ tempo_led_flag<<(step_count));//si play et pas shift button la led defile et le pattern selectionner clignote au tempo
        }
        else{
          temp_step_led=(((1*tempo_led_flag_block)<<selected_pattern) ^ (tempo_led_flag<<(step_count%16))^(temp_led_block ^ temp_led_pattern));
        }
        PORTC = (1<<(button_scale_count+2)) + (1<<(!button_pattern_part+6));//Update les led de la parti edit suivant la scale et le nombre de pas auquel le sequenceur en est
      }
      //PLAY ET SHIFT-------------------------------------------------------
      else if (play && button_shift){
        temp_step_led=(1<<pattern_bank ^ tempo_led_flag<<step_count);//si play et shift button 
        PORTC=B11111100;//allumer toutes leds pour indiquer qu'on selectionne la bank
      }
    }
    //fonction qui permet d'updater la led de la scale en fonction de la scale du pattern
    switch (pattern_scale[pattern_buffer]){
    case 12:
      button_scale_count=0;
      break;
    case 24:
      button_scale_count=1;
      break;
    case 16:
      button_scale_count=2;
      break;
    case 32:
      button_scale_count=3;
      break;
    }
    if (step_count<16 || step_count==255)button_pattern_part=0;//on initialise a 0 quand le pas courant est en dessous de 16 ou en mode stop soit step_count=255  
    else if (step_count>=16)button_pattern_part=1;
  }

  //Mode Pattern Edit
  //-----------------------------------------
  if(selected_mode==PATTERN_EDIT){
    //fonction qui permet d'updater la led de la scale en fonction de la scale du pattern
    switch (pattern_scale[pattern_buffer]){
    case 12:
      button_scale_count=0;
      break;
    case 24:
      button_scale_count=1;
      break;
    case 16:
      button_scale_count=2;
      break;
    case 32:
      button_scale_count=3;
      break;
    }

    if ( (nbr_step[pattern_buffer] > 16)&& button_part_switch )
    {
      if(step_count>=16 && button_pattern_part==0) {
        button_pattern_part=1;
      }
      if(step_count<16 && button_pattern_part==1) {
        button_pattern_part=0;
      }
    }  

    PORTC = (1<<button_scale_count+2) + (1<<!button_pattern_part+6);//Update les led de la parti edit suivant les boutons.

    if(play){
      if(button_pattern_part==0){//si la parti 1 a 16 est selectionner
        temp_step_led=(pattern[pattern_buffer][selected_inst][0]^tempo_led_flag<<step_count); 
      }
      else if(step_count>=16 && button_pattern_part==1){//si la parti 17 a 32 est selectionner et que le compteur step est superieur ou egale a 16
        temp_step_led=(pattern[pattern_buffer][selected_inst][1]^tempo_led_flag<<step_count-16); 
      }
      else if (step_count<16 & button_pattern_part==1){//si la parti 17 a 32 est selectionner et que le compteur step est inferieur  a 16
        temp_step_led=(pattern[pattern_buffer][selected_inst][1]);
      }        
    }
    else if (!play){
      if(button_shift){
        temp_step_led=(1<<pattern_bank);
        PORTC=B11111100;//allumer toutes leds pour indiquer qu'on selectionne la bank
      }
      else if (!button_shift){
        temp_step_led=(1*tempo_led_flag<<(selected_pattern));
      }
    }
  }
}

















