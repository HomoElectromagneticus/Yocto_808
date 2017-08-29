void Update_Pattern_EEprom(){

  //si le pattern a ete editer ou son nbr de step et qu'on est en STOP on sauve la pattern ou si on reviens dans le mode precedent 
  if((selected_pattern_edited_saved ==1|| nbr_step_changed==1 || pattern_scale_changed==1)&& (!play || selected_mode==old_selected_mode)){
    Save_Pattern();
    selected_pattern_edited_saved =0;//reset le flag que le pattern a ete editer
    nbr_step_changed=0;//reset le flag que le nbr de step a change
    pattern_scale_changed=0;//reset le flag que la scale a change
  }

  //si le pattern selectionner a changer on load dans le buffer le pattern suivant

  if( old_pattern_nbr != pattern_nbr) {//&& (pattern_switched)){
    Load_Pattern(); 
  }
}


void Update_Song_EEprom()
{

  //on sauve le song si le old_select_pattern est bien en dessus de 16 et a chaque appuie sur reset ou au changement de song s'il a ete editer 
  
  
  if ( (old_selected_song<16) && ( (selected_song_changed && selected_song_edited) || (first_push_end && selected_song_edited)) ) {
    Save_Song();
    //Serial.println("savesong ok");
    selected_song_changed=0;
    selected_song_edited=0;
  }

  if ( old_selected_song != selected_song ){
    Load_Song();
  }
}










