
//===============================================================================
void Mode_Song_Edit()
{
  Verticalize_Pattern();//cf fonction
  
  unsigned int reading = SR.Button_Step_Read();// on lit les boutons

  if(reading!=old_step_button_state){// si ils ont changer on enregistre le temps
    millis_debounce_step_button=millis();
  }
  if((millis()-millis_debounce_step_button) >= DEBOUNCE ){//apres le temps de debounce on compare la valeur des boutons avec l'ancienne valeur
    if(reading!=step_button_state){
      step_button_state=reading;
      if (button_shift){//si bouton shift appuyer
        for (byte i=0;i<16;i++){//loop autant de fois que de bouton step soit 16
          if (bitRead (step_button_state,i)){
            // si le bouton shift est appuyer on retourne la bank selectionner selectionner
            pattern_bank=i;
            selected_pattern_changed=1;//flag que le pattern selectionner a change
            selected_pattern_changed_A=1;//flag que le pattern selectionner a change
            break;
          }
        }
      }
      else if(!button_shift){
        for (byte i=0;i<16;i++){//loop autant de fois que de bouton step soit 16
          if (bitRead (step_button_state,i)){
            //on retourne la valeur du pattern selectionner
            selected_pattern=i;
            selected_pattern_changed=1;//flag que le pattern selectionner a change sert pour verticalise le pattern (cf verticalize)
            selected_pattern_changed_A=1;//flag que le pattern selectionner a change
            break;
          }
        }
      }
      pattern_nbr = selected_pattern+(16*pattern_bank);//le numero du pattern est egal au pattern selectionner plus 16 fois la bank soit 255 pattern
    }
  }
  old_step_button_state = reading;
  //-------------------------------------------

  if(button_next && (!first_push_next)){
    first_push_next=1;//flag que l'on vient d'appuyer sur next
    nbr_pattern_song++;//on incremente la position du song
    if (nbr_pattern_song >= MAX_PATTERN_SONG){// si la position arrive au max de la position 
      nbr_pattern_song=0;//on reset la position
    }
  }
  if ( selected_pattern_changed_A ) {//Si le pattern selectionner a change
    song[nbr_pattern_song][song_buffer]=pattern_nbr;// on enregistrer le pattern selectionner a la postion du song
    selected_pattern_changed_A=0;//Initialise la deuxieme variable du changement de pattern selectionne
    selected_song_edited=1;//Flag que le song a ete editer
  }
  else {//si il n'a pas change on lit l'array song et on retourne le pattern et la bank selectionner sur les led
    pattern_bank = (song[nbr_pattern_song][song_buffer])/16;//pour retrouver la bank a partir du numero du partir on divise par 16
    selected_pattern = (song[nbr_pattern_song][song_buffer])- (16* pattern_bank);//Pour retrouver le nbr du pattern selectionner on soustrait 16 fois la bank au numero du pattern
    pattern_nbr = (song[nbr_pattern_song][song_buffer]);//enregistre le nouveau numero du pattern
    selected_pattern_changed=1;//flag que le pattern selectionner a change sert pour verticalise le pattern (cf verticalize)
  } 
  //-------------------------------------------
  //ici on enregistre le nbr total de pattern dans le song
  if(button_end && (!first_push_end)){//si on appuie sur le bouton end on enregistre le nbr total de pattern qu'il y a dans le song
    first_push_end=1;//flag que l'on vient d'appuyer sur end
    total_pattern_song[song_buffer] = nbr_pattern_song;
    selected_song_edited=1;//flag que le song a été édité
  }

  //-------------------------------------------
  //le bouton reset ramener au premier pattern du song
  if(button_reset) {
    Reset_Song();
/*    
    nbr_pattern_song=0;//on reset la position
    pattern_nbr = (song[nbr_pattern_song][song_buffer]);//enregistre le nouveau numero du pattern
    pattern_bank = pattern_nbr/16;//pour retrouver la bank a partir du numero du partir on divise par 16
    selected_pattern = pattern_nbr - (16* pattern_bank);//Pour retrouver le nbr du pattern selectionner on soustrait 16 fois la bank au numero du pattern
    selected_pattern_changed=1;//flag que le pattern selectionner a change sert pour verticalise le pattern (cf verticalize)
*/    
  }

  //---------------------------------------------
  //On switch le buffer si le song a ete changer et loader et qu'on est en stop
  if (!play && load_song_ok){
    song_buffer=!song_buffer;
    load_song_ok=0;//reinitialsie le flag que song a ete loader
  }
}

//===============================================================================
void Mode_Song_Play()
{

  Verticalize_Pattern();//cf fonction

  if(middle_mesure_flag) 
  {

    middle_mesure_flag=0;

    //On switch le buffer si le song a ete changer et loader et qu'on est en play a la fin de la mesure du dernier pattern du song

    if (play && load_song_ok && pattern_count==0 ){
      song_buffer=!song_buffer;
      load_song_ok=0;//reinitialsie le flag que song a ete loader
      selected_pattern_changed=1;//le pattern selectionne a change
    }

    middle_mesure_flag=0;
    pattern_nbr = song[pattern_count][song_buffer];
    pattern_bank = pattern_nbr/16;//pour retrouver la bank a partir du numero du partir on divise par 16
    selected_pattern = pattern_nbr - (16* pattern_bank);//Pour retrouver le nbr du pattern selectionner on soustrait 16 fois la bank au numero du pattern
    selected_pattern_changed=1;//le pattern selectionne a change

  }
if(!play){
  if(button_reset) {
    Reset_Song();
  }
}
/*
  if (!play)
  {
    if( button_reset || selected_song_changed_A)
    {
      Reset_Song();
      selected_song_changed_A=0;
    }

    //On switch le buffer si le song a ete changer et loader et qu'on est en stop
    if ( load_song_ok )
    {
        song_buffer=!song_buffer;
        load_song_ok=0;//reinitialsie le flag que song a ete loader
        selected_pattern_changed=1;//le pattern selectionne a change
    }
  }
*/
}


void Reset_Song()
{
    nbr_pattern_song=0;//on reset la position
    pattern_count=0;
    pattern_nbr = (song[nbr_pattern_song][song_buffer]);//enregistre le nouveau numero du pattern
    old_pattern_nbr = pattern_nbr + 1;
    pattern_bank = pattern_nbr / 16;//pour retrouver la bank a partir du numero du partir on divise par 16
    selected_pattern = pattern_nbr % 16;//Pour retrouver le nbr du pattern selectionner on soustrait 16 fois la bank au numero du pattern
    // Pas besoin d'appeler Load PAttern, l'appel suivant Update_Pattern_EEprom() le fait si Old_pattern_Nbr != pattern_Nbr
}






















