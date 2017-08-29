void Update_Song_Led()
{
  if( button_shift){
    temp_step_led=(1<<pattern_bank);
  }
  else 
  {
    if( !play ) {
      temp_step_led=(1*tempo_led_flag<<(selected_pattern));
    }
    else if (play){
      temp_step_led=( ( 1 << selected_pattern ) ^ ( tempo_led_flag<<step_count ));//si play et pas shift button la led defile et le pattern selectionner clignote au tempo
    }
  }
  
  if(selected_mode!=SONG_EDIT){
    PORTC = (pattern_count%16)<<2;
  }
  else{
    PORTC = (nbr_pattern_song%16)<<2;
  }

}



