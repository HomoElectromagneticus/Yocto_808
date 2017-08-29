//La library Wire a du etre modifier pour grossir le buffer jusqu'a 128 byte.
//le fichier twi.h et wire.h on etait modifier : #define BUFFER_LENGTH 128

#define PTRN_SIZE_BYTE (uint16_t)64  //deux octets pour 16 pas fois 16 instruments fois deux pour les deux parti 1a16 et 17a32 
#define PTRN_SETUP_SIZE_BYTE (uint16_t)2 //1 octet pour le nombre pas + 1 octet pour la scale
#define PTRN_NB (uint16_t)256 // 16 * 16 

#define SONG_SIZE_BYTE (uint16_t)256 //taille Max d'un song en octet soit 256 pattern par song
#define SONG_SETUP_SIZE_BYTE (uint16_t)1 // un octet stock le nombre de pattern dans le song
#define SONG_NB (uint16_t)16

#define SIZE_PAGE_MAX 64    

#define OFFSET_PATTERN (uint16_t)( 0 )

#define OFFSET_PATTERN_SETUP ( OFFSET_PATTERN + (PTRN_SIZE_BYTE * PTRN_NB) ) // 16384

#define OFFSET_SONG ( OFFSET_PATTERN_SETUP +  (PTRN_SETUP_SIZE_BYTE  * PTRN_NB) )  // 16896

#define OFFSET_SONG_SETUP ( OFFSET_SONG +  ( SONG_SIZE_BYTE  * SONG_NB) )  // 20992 

#define OFFSET_MIDI_NOTE ( OFFSET_SONG_SETUP + ( SONG_SETUP_SIZE_BYTE * SONG_NB ) )  // 21008

#define OFFSET_CHANNEL ( OFFSET_MIDI_NOTE + 16 ) // 21024

#define DELAY_WR 5
//======================================================================
void Save_Song()
{
  byte nbr_page = (SONG_SIZE_BYTE / SIZE_PAGE_MAX);
  unsigned int adress = OFFSET_SONG + old_selected_song * SONG_SIZE_BYTE;
  unsigned int offset = 0;
  for (char i=0 ;i <=nbr_page ; i++) 
  {
    Wire_Begin_TX(adress+offset);
    for(char j=0;j<SIZE_PAGE_MAX;j++)
    {
      Wire.write( (byte)song[offset+j][song_buffer]);//envoi la valeur du numero de pattern correspondant a l'address
    }
    Wire.endTransmission();
    delay(DELAY_WR);//neccessaire avant d'ecrire une autre page
    offset+=SIZE_PAGE_MAX;
  }
  adress = OFFSET_SONG_SETUP + old_selected_song * SONG_SETUP_SIZE_BYTE;
  Wire_Begin_TX(adress);
  Serial.println(total_pattern_song[song_buffer]);
  Wire.write ( (byte)total_pattern_song[song_buffer]);
  Wire.endTransmission();
  delay(DELAY_WR);//neccessaire avant d'ecrire une autre page
}

//======================================================================
void Load_Song()
{
  byte nbr_page = (SONG_SIZE_BYTE / SIZE_PAGE_MAX);
  unsigned int adress = OFFSET_SONG + selected_song * SONG_SIZE_BYTE;
  unsigned int offset = 0;

  for (char i=0;i<nbr_page;i++) { //loop autant de fois que de page de 64byte
    Wire_Begin_TX(adress+offset);
    Wire.endTransmission();
    Wire.requestFrom(0x50,SIZE_PAGE_MAX); //request des 64octets de la page du song
    for(char j=0;j<SIZE_PAGE_MAX;j++){//loop 64 fois pour ecrire la page
      song[offset+j][!song_buffer]= Wire.read();//load dans le variable a l'adress de l'array le numero du pattern
    }
    offset+=SIZE_PAGE_MAX;
  }

  adress = OFFSET_SONG_SETUP + selected_song * SONG_SETUP_SIZE_BYTE;
  Wire_Begin_TX(adress);
  Wire.endTransmission();
  Wire.requestFrom(0x50,SONG_SETUP_SIZE_BYTE); 
  total_pattern_song[!song_buffer]= Wire.read(); 
  old_selected_song=selected_song;

  if (play)
  {
    load_song_ok = 1;
    selected_song_changed = 1; 
  }
  else
  {
    song_buffer=!song_buffer;
    load_song_ok=0;//reinitialsie le flag que song a ete loader
    Reset_Song();
    Load_Pattern();
  }
}

//======================================================================
void Save_Pattern()
{
  unsigned int adress = OFFSET_PATTERN + old_pattern_nbr * PTRN_SIZE_BYTE;

  Wire_Begin_TX(adress); //commence la transmission a l'adress du pattern selectionner

  for (char i =0;i<NBR_INST;i++){//loop autant de fois que d'instruments
    for (char j=0;j<2;j++){//loop deux fois pour les deux parti du pattern 1a16 et 17 a 32
      byte lowbyte = ((pattern[pattern_buffer][i][j]>>0)& 0xFF);//stock les 8 premier bits du pattern
      byte highbyte = ((pattern[pattern_buffer][i][j]>>8)& 0xFF);// et les 8 bit "fort"
      Wire.write( (byte)lowbyte);//envoi
      Wire.write( (byte)highbyte);
    }
  }

  Wire.endTransmission();
  delay(DELAY_WR);//neccessaire avant d'ecrire une autre page

  adress = OFFSET_PATTERN_SETUP + old_pattern_nbr * PTRN_SETUP_SIZE_BYTE;
  Wire_Begin_TX(adress);//commence la transmission a l'adress du pattern selectionner
  Wire.write( (byte)nbr_step[pattern_buffer]);//envoi le nombre de step
  Wire.write( (byte)pattern_scale[pattern_buffer]);//envoi la scale du pattern
  Wire.endTransmission();
  delay(DELAY_WR);//neccessaire avant d'ecrire une autre page
}

//======================================================================
void Save_Paste_Pattern()
{
  unsigned int adress = OFFSET_PATTERN + pattern_nbr * PTRN_SIZE_BYTE;
  Wire_Begin_TX(adress);//commence la transmission a l'adress du pattern selectionner
  for (char i =0;i<NBR_INST;i++){//loop autant de fois que d'instruments
    for (char j=0;j<2;j++){//loop deux fois pour les deux parti du pattern 1a16 et 17 a 32
      byte lowbyte = ((copy_pattern_buffer[i][j]>>0)& 0xFF);//stock les 8 premier bits du pattern
      byte highbyte = ((copy_pattern_buffer[i][j]>>8)& 0xFF);// et les 8 bit "fort"
      Wire.write( (byte)lowbyte);//envoi
      Wire.write( (byte)highbyte);
    }
  }
  Wire.endTransmission();
  delay(DELAY_WR);//delay necessaire pour le bonne enregistrement du setup pattern

  adress = OFFSET_PATTERN_SETUP + pattern_nbr * PTRN_SETUP_SIZE_BYTE;
  Wire_Begin_TX(adress);//commence la transmission a l'adress 16384 qui correspond au 16pattern*16Bank*64octets
  Wire.write( (byte)copy_pattern_nbr_step);//envoi le nombre de step
  Wire.write( (byte)copy_pattern_scale);//envoi la scale du pattern
  Wire.endTransmission();
  delay(DELAY_WR);//delay necessaire pour le bonne enregistrement du setup pattern
}

//======================================================================
void Load_Pattern()
{
  unsigned int adress = OFFSET_PATTERN + pattern_nbr * PTRN_SIZE_BYTE;
  Wire_Begin_TX(adress);//commence la transmission a l'adress du pattern selectionner
  Wire.endTransmission();
  Wire.requestFrom(0x50,PTRN_SIZE_BYTE); //request des 64octets du pattern a l'adresse de l'eeprom
  for (char i =0;i<NBR_INST;i++){//loop autant de fois que d'instruments
    for (char j=0;j<2;j++){//loop deux fois pour les deux parti du pattern 1a16 et 17 a 32
      pattern[!pattern_buffer][i][j]=((Wire.read() << 0) & 0xFF) + ((Wire.read() << 8) & 0xFF00);//on load le pattern dans l'autre parti du buffer
    }
  }

  adress = OFFSET_PATTERN_SETUP + pattern_nbr * PTRN_SETUP_SIZE_BYTE;
  Wire_Begin_TX(adress);//commence la transmission a l'adress du pattern selectionner
  Wire.endTransmission();
  Wire.requestFrom(0x50,PTRN_SETUP_SIZE_BYTE); //request des 2 octets du pattern setup a l'adresse de l'eeprom
  nbr_step[!pattern_buffer]=Wire.read();
  pattern_scale[!pattern_buffer]=Wire.read();
  old_pattern_nbr  = pattern_nbr;

  if (play)
  {
    load_pattern_ok = 1;
    selected_pattern_changed = 1; // Pour forcer la verticalisation
  }
  else
  {
    Verticalize_Pattern_After_Cleared();
    pattern_buffer=!pattern_buffer;//on switche entre les deux pattern present dans le buffer au debut de la mesure
    load_pattern_ok=0;//reinitialise le flag de load pattern
    selected_pattern_changed = 0; 
    selected_pattern_edited = 0;
  }
}

//======================================================================
void Clear_EEprom_Pattern()
{
  unsigned int adress = OFFSET_PATTERN + pattern_nbr * PTRN_SIZE_BYTE;
  Wire_Begin_TX(adress);//commence la transmission a l'adress du pattern selectionner
  for (char i =0;i<NBR_INST;i++){//loop autant de fois que d'instruments
    for (char j=0;j<2;j++){//loop deux fois pour les deux parti du pattern 1a16 et 17 a 32
      byte lowbyte = 0;//clear
      byte highbyte = 0;// clear
      Wire.write( (byte)lowbyte);//envoi
      Wire.write( (byte)highbyte);
    }
  }
  Wire.endTransmission();
  delay(DELAY_WR);

  adress = OFFSET_PATTERN_SETUP + pattern_nbr * PTRN_SETUP_SIZE_BYTE;
  Wire_Begin_TX(adress);//commence la transmission a l'adress 16384 qui correspond au 16pattern*16Bank*64octets
  Wire.write( (byte)16);//initialise le pattern a 16 pas
  Wire.write( (byte)24);//initialise le pattern avec une scale de 1/16 soit 24 pulses entre chaque pas
  Wire.endTransmission();
  delay(DELAY_WR);

}

//fonction qui initialise l'eeprom avec des pattern de 16 pas et une scale 1/16

struct _samplePattern {        
  byte    Step;      
  byte    Scale;
  unsigned long Instr[NBR_INST];
};


_samplePattern FactoryLib [16] = { 
  { 
    32, 24, /* Sexual Healing TR-808 drum pattern – by SynthManiaDotCom*/
    0b10000000000010000000000000000000,  /* AC */
    0b10000001101010011000000010000001,  /* BD */ 
    0b00000000001100000000001101010100,  /* SD */
    0b00000000000000000000000000001010,  /* LT */
    0b00000000000000000000001000000000,  /* MT */
    0b00100000000000000010000000000000,  /* HT */
    0b10010010000000001001001000100100,  /* RS */
    0b00001000000000000000100000000000,  /* CP */
    0b00000000000000000000000000000000,  /* CB */
    0b00000000000000000000000000000000,  /* CY */
    0b00000010000000000010001000100100,  /* OH */
    0b10111010101010111010101011101011,  /* CH */
    0,  /* TR1 */
    0,  /* TR2 */
    0,  /* TR3 */
    0,   /* NC */
  }
  ,
  { 
    16,24, 
    0b00001000100000000000000000000000,  /* AC */
    0b10000000001101000000000000000000,  /* BD */ 
    0b00001000100000000000000000000000,  /* SD */
    0b00000000000000000000000000000000,  /* LT */
    0b00000000000000000000000000000000,  /* MT */
    0b00000000000000000000000000000000,  /* HT */
    0b10100010001000100000000000000000,  /* RS */
    0b00000000000000000000000000000000,  /* CP */
    0b00000000000000000000000000000000,  /* CB */
    0b00000000000000000000000000000000,  /* CY */
    0b00101000000000000000000000000000,  /* OH */
    0b00010000001000100000000000000000,  /* CH */
    0,  /* TR1 */
    0,  /* TR2 */
    0,  /* TR3 */
    0,   /* NC */
  }
  ,
  {
    32,24,
    0b10001000100010001000100010001000,
    0b10000010001000000000001001000000,
    0b00001000010010000000100000001001,
    0b01100000000000000001000000000000,
    0b00010000000100010000100000000000,
    0b00010000100000000000000100000000,
    0b00000001010000000001000101000001,
    0b00000000100000000000100000001000,
    0b01000001010000010010000101000001,
    0b10000000000000000000000000000000,
    0b00000001000000000000000101000000,
    0b10101010101010101010101010101010,
    0,
    0,
    0,
    0,
  }
  ,
  {
    32,24,
    0b10001000100010001000100010001000,
    0b10011000100010001000100010001010,
    0b00001000000010000000100000001000,
    0b00000000000001110000000000000000,
    0b00000001000000000000000100000000,
    0b10010000000000000000000000000001,
    0b00000001010000000000000101000001,
    0b00000000100000000000000010000000,
    0b00100000111000000001000011100001,
    0b00000000010000100000000001000010,
    0b00100000000000000010000000000000,
    0b10001010101010101100101010101010,
    0,
    0,
    0,
    0,
  }
  ,
  {
    16,24,
    0b10001000100010000000000000000000,
    0b10010010001001000000000000000000,
    0b00001000000010000000000000000000,
    0b00000000000101000000000000000000,
    0b00000001000000000000000000000000,
    0b01100000000000000000000000000000,
    0b00000011000000010000000000000000,
    0b00000000100000000000000000000000,
    0b00000100000000010000000000000000,
    0b10000000000000000000000000000000,
    0b00100001000001000000000000000000,
    0b10001000100010000000000000000000,
    0,
    0,
    0,
    0,
  }
  ,
  {
    16,24,
    0b10000000100000000000000000000000,
    0b10000000001001000000000000000000,
    0b00001000000000100000000000000000,
    0b00000000000010010000000000000000,
    0b00000000010100000000000000000000,
    0b00000001000000000000000000000000,
    0b01100000000101000000000000000000,
    0b00000000100000000000000000000000,
    0b00000001000001000000000000000000,
    0b00000000000000000000000000000000,
    0b10000001000010000000000000000000,
    0b00101010101010100000000000000000,
    0,
    0,
    0,
    0,
  }
  ,
  {
    16,24,
    0b10001010100010100000000000000000,
    0b10010010000000000000000000000000,
    0b00000000100000100000000000000000,
    0b00000000001000000000000000000000,
    0b00000001000000000000000000000000,
    0b01000000000000000000000000000000,
    0b01000001000000000000000000000000,
    0b00001000000010000000000000000000,
    0b01000000000010000000000000000000,
    0b00000000000000000000000000000000,
    0b10000000000000000000000000000000,
    0b00100011001000100000000000000000,
    0,
    0,
    0,
    0,
  }
  ,
  {
    16,24,
    0b10001000100010000000000000000000,
    0b10100001101000000000000000000000,
    0b00000001000010000000000000000000,
    0b00000001000000000000000000000000,
    0b00000100000000000000000000000000,
    0b00001000000000000000000000000000,
    0b00000010000000100000000000000000,
    0b00001000000001000000000000000000,
    0b00000010000001000000000000000000,
    0b00000000000000000000000000000000,
    0b00010000010000000000000000000000,
    0b10001000100010000000000000000000,
    0,
    0,
    0,
    0,
  }
  ,
  {
    32,24,
    0b10010010010010001000100010001000,
    0b10010010010010001001001010001000,
    0b00000000100000000000000010000000,
    0b00000000000000000000000000000000,
    0b00000000000000000000000000000000,
    0b00000000000000000000000000000000,
    0b10001010000100001001011101001010,
    0b00000000100000000000000010000000,
    0b00000000000000000000000000000000,
    0b10000000000000000000000000000000,
    0b01000010010001000000100010100000,
    0b10101010101010101001011101011010,
    0,
    0,
    0,
    0,
  }
  ,
  {
    32,24,
    0b10000010101001001000100010000100,
    0b10000100000100101000000000000000,
    0b00000000100000000000000010000000,
    0b00000000000100000000000000010100,
    0b00000000100100000000000000000000,
    0b00100010000000000000000000000000,
    0b10010010010000101110001000100100,
    0b00000000000000000000000010000000,
    0b00000000000000000000000000000000,
    0b00000000000000000000000000000000,
    0b00010100000000000000010100000000,
    0b01001010000010101110000010101010,
    0,
    0,
    0,
    0,
  }
  ,
  {
    16,24,
    0b00001000000010000000000000000000,
    0b10000010000101000000000000000000,
    0b00001000000010000000000000000000,
    0b00100000010000000000000000000000,
    0b00000000001000000000000000000000,
    0b00000000000000000000000000000000,
    0b00000000010000000000000000000000,
    0b00000001000000000000000000000000,
    0b00000000000000100000000000000000,
    0b00000000000000000000000000000000,
    0b00100001000010000000000000000000,
    0b10001110011100110000000000000000,
    0,
    0,
    0,
    0,
  }
  ,
  {
    16,24,
    0b00000000000000000000000000000000,
    0b10000010000000100000000000000000,
    0b00100010000010000000000000000000,
    0b11010000000000000000000000000000,
    0b00000101000000000000000000000000,
    0b00000000100100000000000000000000,
    0b00000000000010100000000000000000,
    0b00000000000000000000000000000000,
    0b00000000000000000000000000000000,
    0b00000000000000000000000000000000,
    0b00000000000000100000000000000000,
    0b10101010101010000000000000000000,
    0,
    0,
    0,
    0,
  }
  ,
  {
    12,32,
    0b10001001001000000000000000000000,
    0b10010010010000000000000000000000,
    0b00010000010000000000000000000000,
    0b00101000000000000000000000000000,
    0b00000010000000000000000000000000,
    0b00000000010100000000000000000000,
    0b01000100001000000000000000000000,
    0b00000000000000000000000000000000,
    0b00000000000000000000000000000000,
    0b00000000000000000000000000000000,
    0b00000000000000000000000000000000,
    0b11111111111100000000000000000000,
    0,
    0,
    0,
    0,
  }
  ,
  {
    12,32,
    0b00010000010000000000000000000000,
    0b10000100101000000000000000000000,
    0b00010000010000000000000000000000,
    0b00000001000000000000000000000000,
    0b00100000100000000000000000000000,
    0b01000000000000000000000000000000,
    0b00000001000000000000000000000000,
    0b00000000010000000000000000000000,
    0b00000010000000000000000000000000,
    0b00000000000000000000000000000000,
    0b01000000010000000000000000000000,
    0b10101010100100000000000000000000,
    0,
    0,
    0,
    0,
  }
  ,
  {
    24,32,
    0b10010010010010010010010000000000,
    0b10010010010010010010010000000000,
    0b10100001001000010001001000000000,
    0b00000001100001000100000000000000,
    0b00001000000100001001101100000000,
    0b10100000000000000000000000000000,
    0b00010001000000010001000100000000,
    0b00001000010000001000010000000000,
    0b10010010010010010010010000000000,
    0b01000000000000000000000000000000,
    0b01000010000001000010000000000000,
    0b00100100100100100100100100000000,
    0,
    0,
    0,
    0,
  }
  ,
  {
    15,32,
    0b10010010010010000000000000000000,
    0b10010010010010000000000000000000,
    0b00100010001000100000000000000000,
    0b00000000000001000000000000000000,
    0b01100000000000000000000000000000,
    0b10000010000010000000000000000000,
    0b00010000000010000000000000000000,
    0b00000100000001000000000000000000,
    0b00001000000010000000000000000000,
    0b10000000000000000000000000000000,
    0b00000100000001000000000000000000,
    0b10101010101010100000000000000000,
    0,
    0,
    0,
    0,
  }

};


void Initialize_EEprom()
{
  int temp=0;
  int x;

  if (button_init){

    button_init=0;//reinitialise le bouton init pour eviter que la loop se fasse deux fois


    //loop autant de fois que de nombre de pattern
    for ( x=0; x< PTRN_NB ;x++){

      unsigned int adress = OFFSET_PATTERN + x * PTRN_SIZE_BYTE;
      Wire_Begin_TX(adress);//commence la transmission a l'adress 16384 qui correspond au 16pattern*16Bank*64octets

        for (char i =0;i<NBR_INST;i++) {
        {
          unsigned long rmask = 0x80000000;
          unsigned long smask = 1;
          unsigned long resul = 0;

          if ( x < 16 ) // on prend un FactoryLib si x < ....
          {
            while (rmask)
            {
              if ( ( FactoryLib[x].Instr[i] & rmask ) == rmask )  resul |= smask;
              rmask>>=1;
              smask<<=1;
            }
          }

          byte c = ( resul & 0x000000FF );
          Wire.write( (byte)c);//envoi
          c = ( resul & 0x0000FF00 ) >> 8;
          Wire.write( (byte)c);//envoi
          c = ( resul & 0x00FF0000 ) >> 16;
          Wire.write( (byte)c);//envoi
          c = ( resul & 0xFF000000 ) >> 24;
          Wire.write( (byte)c);//envoi
        }

      }
      Wire.endTransmission();
      delay(DELAY_WR);

      adress = OFFSET_PATTERN_SETUP + x * PTRN_SETUP_SIZE_BYTE;
      Wire_Begin_TX(adress);
      if ( x < 16 ) // on prend un FactoryLib si x < ....
      {
        Wire.write(  (byte)FactoryLib[x].Step );//initialise le pattern a 16 pas
        Wire.write(  (byte)FactoryLib[x].Scale );//initialise le pattern a 16 pas
      }
      else
      {  
        Wire.write( (byte)16 );//initialise le pattern a 16 pas
        Wire.write( (byte)24 );//initialise le pattern avec une scale de 1/16 soit 24 pulses entre chaque pas
      }
      Wire.endTransmission();
      delay(DELAY_WR);

      temp|=1<<(x/16);
      SR.Led_Step_Write(temp);
    }
    delay(100);
    SR.Led_Step_Write(0);

    // On initialise les SONG 
    temp=0;
    for (x=0; x < SONG_NB;x++)
    {
      byte nbr_page = (SONG_SIZE_BYTE / SIZE_PAGE_MAX);
      unsigned int adress = OFFSET_SONG + x * SONG_SIZE_BYTE;
      for (char i=0; i < nbr_page;i++) 
      {
        Wire_Begin_TX(adress);
        for(char j=0;j<SIZE_PAGE_MAX;j++)
        {
          Wire.write((byte)0);//envoi la valeur du numero de pattern correspondant a l'address
        }
        Wire.endTransmission();
        delay(DELAY_WR);//neccessaire avant d'ecrire une autre page
        adress+=SIZE_PAGE_MAX;
      }

      adress = OFFSET_SONG_SETUP + x * SONG_SETUP_SIZE_BYTE;
      Wire_Begin_TX(adress);
      Wire.write((byte)4);//sauve le nombre de pattern dans le song
      Wire.endTransmission();
      delay(DELAY_WR);//necessaire avant d'ecrire une autre page
      temp|=1<<x;
      SR.Led_Step_Write(temp);
    }
    delay(100);
    SR.Led_Step_Write(0);

    // On intialise les notes par défaut
    defNotes();
    inst_midi_note_edited=1;
    Save_Midi_Note();

    // On intialise lechanel par defaut
    selected_channel=0;
    Save_Midi_Channel();   

    // repasse en 0 pour pattern et bank
    selected_pattern=0;
    pattern_bank=0;
    pattern_nbr=0;
    Load_Pattern();//on load le pattern 0


  }
  button_init=0;//reinitialise le bouton init pour eviter que la loop se fasse deux fois
}

//======================================================================
//Ici on sauve le numero des note MIDI des instru
void Save_Midi_Note()
{
  if(inst_midi_note_edited)
  {
    inst_midi_note_edited=0;
    Wire_Begin_TX(OFFSET_MIDI_NOTE);
    for(int i=0;i<16;i++)
    {
      Wire.write((byte)inst_midi_note[i]);//envoi le numero des note midi
    }
    Wire.endTransmission();
    delay(DELAY_WR);
  }
}

//======================================================================
//Load Midi parameter
void Load_Midi_Note()
{
  Wire_Begin_TX(OFFSET_MIDI_NOTE);
  Wire.endTransmission();
  Wire.requestFrom(0x50,16); //request 16 pour les notes des instru 
  for(int i=0;i<16;i++)
  {
    inst_midi_note[i]=Wire.read();//envoi le numero des note midi
  }
}

//===================================================
//Ici on sauve le numero dub channel midi
void Save_Midi_Channel()
{ 
  Wire_Begin_TX(OFFSET_CHANNEL);
  Wire.write((byte)selected_channel);//envoi le numero des note midi
  Wire.endTransmission();
  delay(DELAY_WR);
}

//======================================================================
//Load Midi Channel
void Load_Midi_Channel()
{
  Wire_Begin_TX(OFFSET_CHANNEL);
  Wire.endTransmission();
  Wire.requestFrom(0x50,1); 
  selected_channel=Wire.read();//envoi le numero des note midi
  if (selected_channel>15) selected_channel=0;//si le canal est superieur a 16 on choisi le 1
}


void Wire_Begin_TX(uint16_t address)
{
  Wire.beginTransmission(0x50);
  Wire.write( (byte)(address >> 8));
  Wire.write( (byte)(address & 0xFF));
}


void Play_Version()
{
  if (button_reset)
  {
    byte count = 0;
    while ( count < major_version )
    {
      PORTC |=  (1 << 7);     
      PORTB |= (1<<2);//envoie une impulsion sur la sorti trig CPU a chaque pas
      SR.ShiftOut_Update(temp_step_led,0b10);
      delay(200);
      PORTC &=  ~(1 << 7);     
      PORTB &= ~(1<<2);//envoie une impulsion sur la sorti trig CPU a chaque pas
      SR.ShiftOut_Update(temp_step_led,0);
      delay(100);
      count++;
    }
    count=0;
    while ( count < minor_version )
    {
      PORTC |=  (1 << 6);     
      PORTB|= (1<<2);// met a 0 la sorti TRIG CPU
      SR.ShiftOut_Update(temp_step_led,0b10000000);
      delay(200);
      PORTC &=  ~(1 << 6);     
      PORTB &= ~(1<<2);//envoie une impulsion sur la sorti trig CPU a chaque pas
      SR.ShiftOut_Update(temp_step_led,0);
      delay(100);
      count++;
    }
  }
  button_reset=0;
}


















