//Pin de l'encoder des BPM
#define ENC_A    30
#define ENC_B    31
#define ENC_SW   10
#define ENC_PORT PINA  //le port sur lequel l'encoder est connecte


// TapTempo définitions
#define TEMPO_MAX  300
#define TEMPO_MIN  40
#define TAPTEMPO_MAX 3  // NBr de valeurs moyennées
uint32_t tapTempo[TAPTEMPO_MAX];
byte tap_button_shift=0;
uint16_t TapTempoBpm=TEMPO_MIN;


//Fonction qui check l'encoder et renvoi le temp dans le timer//
void Check_BPM()
{
  //Local Variables
  int8_t tmpdata;
  //Appel la fonction qui lit l'encoder    
  tmpdata = Read_Encoder();
if(!roll_mode){
  //l'encoder a t il bouger
  if(tmpdata) 
  {

    //Serial.print("Tempo =");
    //Serial.println(bpm/4, DEC);
    //Set the new counter value of the encoder
    bpm -= tmpdata;  
    bpm = constrain(bpm,200,1000); 
    timer_time =((unsigned int)(2500000/bpm));
    //Serial.println(timer_time);
    Timer1.initialize(timer_time); // set a timer of length in microseconds
      
  }
}
}

void Check_Roll_Scale()
{
  //Local Variables
  int8_t tmp_data;
  //Appel la fonction qui lit l'encoder    
  tmp_data = Read_Encoder();

  //l'encoder a t il bouger
  if(tmp_data) 
  {
    if(roll_mode){
      temp_roll_scale -=tmp_data;
      temp_roll_scale=constrain(temp_roll_scale,1,30);
      if(selected_mode==0){//si on est en PATTERN_MIDI_MASTER
      roll_pointer=map(temp_roll_scale,0,30,0,3);
      }
      else{
        roll_pointer=map(temp_roll_scale,0,30,0,2);//on map que entre 0 et 2 en mode slave
      }
      Serial.println(temp_roll_scale);
    }
  }
}

int8_t Read_Encoder()
{
  int8_t enc_states[] = {
    0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0                };
  static uint8_t old_AB = 0;
  /**/
  old_AB <<= 2;                   //remember previous state
  old_AB |= ( ENC_PORT>>6 & 0x03 );  //add current state
  return ( enc_states[( old_AB & 0x0f )]);
}

void TapeTempoInit()
{
      memset(tapTempo,0, sizeof(tapTempo));
      tap_button_shift=0;
}

void TestTapeTempo()
{
    int ct, nbtap, tot;
    int mls;
    if ( !mute_mode )
    {
      if ( button_encoder==1 && tap_button_shift==0)
      {
          tap_button_shift = 1;
          
          for (ct=TAPTEMPO_MAX-1;ct>0;ct--) tapTempo[ct]=tapTempo[ct-1]; 
          tapTempo[ 0 ] = millis();
          nbtap=0;
          tot=0;
          for ( int ct=0;ct<TAPTEMPO_MAX-1; ct++ )
          {
            mls = tapTempo[ ct ] - tapTempo[ ct + 1];
            if ( mls < (60000/TEMPO_MIN) &&  mls >  (60000/TEMPO_MAX) )
            {
              tot +=  mls;
              nbtap++;
            }
          }
          if ( nbtap > 0 )  
          { 
            TapTempoBpm = (uint16_t)60000 / ( tot / nbtap );
            if ( TapTempoBpm< TEMPO_MIN ) TapTempoBpm=TEMPO_MIN;
            if ( TapTempoBpm > TEMPO_MAX ) TapTempoBpm=TEMPO_MAX;
            bpm = TapTempoBpm * 4;
            timer_time =((unsigned int)(2500000/bpm));
            Timer1.initialize(timer_time); // set a timer of length in microseconds
          }
      }
      else if ( button_encoder==0 && tap_button_shift==1)
      {
         tap_button_shift = 0;
      }
   }
}





