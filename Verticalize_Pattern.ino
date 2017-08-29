//=========================================================================================================
//fonction qui verticalize le pattern CaD qui prend chaque bit de chaque pas pour en faire un mot de 16bits
//=========================================================================================================

void Verticalize_Pattern_Init()
{

  if ( selected_pattern_edited || selected_pattern_changed ){
    selected_pattern_changed=0;
    for (byte i=0;i<16;i++){//loop autant de fois que de step par parti soit 16
      inst_step_buffer[i][!pattern_buffer]=0;//initialize a 0 le buffer
      for(byte j=0;j<16;j++){//loop autant de fois que d'instrument soit 16
        inst_step_buffer[i][!pattern_buffer]^=(bitRead(pattern[!pattern_buffer][j][0],i)<<j);
      }
    }
    for (byte i=16;i<32;i++){//loop autant de fois que de step par parti soit 16
      inst_step_buffer[i][!pattern_buffer]=0;//initialize a 0 le buffer
      for(byte j=0;j<16;j++){//loop autant de fois que d'instrument soit 16
        inst_step_buffer[i][!pattern_buffer]^=(bitRead(pattern[!pattern_buffer][j][1],i-16)<<j);
      }
    }
  }
  if((!play) && (old_pattern_nbr != pattern_nbr)){
    pattern_buffer=!pattern_buffer;//permet de switcher entre les deux pattern present dans le buffer au debut de la mesure
    old_pattern_nbr=pattern_nbr;
  } 
}

void Verticalize_Pattern()
{

  //Si le pattern selectionner a changer on le verticalize le pattern qui a été loader dans le buffer, dans le buffer de l'inst et il sera jouer au debut de la mesure suivante
  if  ( selected_pattern_changed ) {

    selected_pattern_changed=0;

    //for (byte x=0;x<2;x++)//loop autant de fois que parti soit 2
    for (byte i=0;i<16;i++){//loop autant de fois que de step par parti soit 16
      inst_step_buffer[i][!pattern_buffer]=0;//initialize a 0 le buffer
      for(byte j=0;j<16;j++){//loop autant de fois que d'instrument soit 16
        inst_step_buffer[i][!pattern_buffer]^=(bitRead(pattern[!pattern_buffer][j][0],i)<<j);
      }
    }
    for (byte i=16;i<32;i++){//loop autant de fois que de step par parti soit 16
      inst_step_buffer[i][!pattern_buffer]=0;//initialize a 0 le buffer
      for(byte j=0;j<16;j++){//loop autant de fois que d'instrument soit 16
        inst_step_buffer[i][!pattern_buffer]^=(bitRead(pattern[!pattern_buffer][j][1],i-16)<<j);
      }
    }
  }
  
  //Par contre si le pattern a été editer on verticalize le pattern joué dans l'inst joué
  if ( selected_pattern_edited ) { 
    selected_pattern_edited=0;
    for (byte i=0;i<16;i++){//loop autant de fois que de step par parti soit 16
      inst_step_buffer[i][pattern_buffer]=0;//initialize a 0 le buffer
      for(byte j=0;j<16;j++){//loop autant de fois que d'instrument soit 16
        inst_step_buffer[i][pattern_buffer]^=(bitRead(pattern[pattern_buffer][j][0],i)<<j);
      }
    }
    for (byte i=16;i<32;i++){//loop autant de fois que de step par parti soit 16
      inst_step_buffer[i][pattern_buffer]=0;//initialize a 0 le buffer
      for(byte j=0;j<16;j++){//loop autant de fois que d'instrument soit 16
        inst_step_buffer[i][pattern_buffer]^=(bitRead(pattern[pattern_buffer][j][1],i-16)<<j);
      }
    }
  }
  //Si on est en mode stop et que le pattern selectionné a changé on switch le buffer
  //En mode play le switch du buffer se fait dans l'interruption pour etre calé au debut de la mesure
  if( !play && load_pattern_ok ) {
    //Serial.println("load_pattern ok");
    pattern_buffer=!pattern_buffer;//permet de switcher entre les deux pattern present dans le buffer au debut de la mesure
    load_pattern_ok=0;
  } 

}

void Verticalize_Pattern_After_Cleared()
{

  //Si le pattern selectionner a changer on le verticalize le pattern qui a été loader dans le buffer, dans le buffer de l'inst et il sera jouer au debut de la mesure suivante
  //for (byte x=0;x<2;x++)//loop autant de fois que parti soit 2
  for (byte i=0;i<16;i++){//loop autant de fois que de step par parti soit 16
    inst_step_buffer[i][!pattern_buffer]=0;//initialize a 0 le buffer
    for(byte j=0;j<16;j++){//loop autant de fois que d'instrument soit 16
      inst_step_buffer[i][!pattern_buffer]^=(bitRead(pattern[!pattern_buffer][j][0],i)<<j);
    }
  }
  for (byte i=16;i<32;i++){//loop autant de fois que de step par parti soit 16
    inst_step_buffer[i][!pattern_buffer]=0;//initialize a 0 le buffer
    for(byte j=0;j<16;j++){//loop autant de fois que d'instrument soit 16
      inst_step_buffer[i][!pattern_buffer]^=(bitRead(pattern[!pattern_buffer][j][1],i-16)<<j);
    }
  } 
}


void Verticalize_Pattern_After_Paste()
{

  //Si le pattern selectionner a changer on le verticalize le pattern qui a été loader dans le buffer, dans le buffer de l'inst et il sera jouer au debut de la mesure suivante
  //for (byte x=0;x<2;x++)//loop autant de fois que parti soit 2
  for (byte i=0;i<16;i++){//loop autant de fois que de step par parti soit 16
    inst_step_buffer[i][pattern_buffer]=0;//initialize a 0 le buffer
    for(byte j=0;j<16;j++){//loop autant de fois que d'instrument soit 16
      inst_step_buffer[i][pattern_buffer]^=(bitRead(pattern[pattern_buffer][j][0],i)<<j);
    }
  }
  for (byte i=16;i<32;i++){//loop autant de fois que de step par parti soit 16
    inst_step_buffer[i][pattern_buffer]=0;//initialize a 0 le buffer
    for(byte j=0;j<16;j++){//loop autant de fois que d'instrument soit 16
      inst_step_buffer[i][pattern_buffer]^=(bitRead(pattern[pattern_buffer][j][1],i-16)<<j);
    }
  } 
}









