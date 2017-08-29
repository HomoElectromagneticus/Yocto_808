//Liste des differents modes selectionner dans la variable "select_mode" 
#define PATTERN_MIDI_MASTER 0
#define PATTERN_DIN_SLAVE   1
#define PATTERN_MIDI_SLAVE  2
#define PATTERN_EDIT        3
#define SONG_MIDI_MASTER 4
#define SONG_DIN_SLAVE   5 
#define SONG_MIDI_SLAVE  6
#define SONG_EDIT        7
#define MIDI_PLAY        8
#define INIT_EEPROM      11
#define CLEAR_PATTERN    13
#define PASTE_PATTERN    14
#define COPY_PATTERN     15
#define CLEAR_SONG       12
#define USER1            9
#define USER2            10



//liste des instruments
#define AC 0
#define BD 1
#define SN 2
#define LT 3
#define MT 4
#define HT 5
#define RM 6
#define HC 7
#define CW 8
#define CY 9
#define OH 10
#define CH 11
#define TRIG1 12 //sorti trigger 1
#define TRIG2 13 //sorti trigger 2
#define TRIG3 14 //Midi instrument 1
#define EXT1  15 //Midi instrument 2


//Fonction qui scan les deux rotary switch du mode et de l'instrument selectionner
void Check_Menu_Inst()
{
  digitalWrite(24,LOW);//met le commun 1 des rotary switch a 1 
  digitalWrite(25,HIGH);//met le commun 2 des Rotary switch a 0 
  selected_inst = selected_song = ((PINA >>2)&~B110000);

  //utiliser pour savoir si on a changer de song selectioner pour pouvoir sauver le song precedent
  if( next_selected_song != selected_song ){
    selected_song_changed=1;
    next_selected_song=selected_song;
  }

  digitalWrite(24,HIGH);//met le commun 1 des rotary switch a 0 
  digitalWrite(25,LOW);//met le commun 2 des rotary switch a 1 
  selected_mode = ((PINA >>2)&~B110000);

}

/*

void Check_Menu_Song()
 {
 digitalWrite(24,HIGH);//met le commun 1 des rotary switch a 1 
 digitalWrite(25,LOW);//met le commun 2 des Rotary switch a 0 
 selected_song = ((PINA >>2)&~B110000);
 digitalWrite(24,LOW);//met le commun 1 des rotary switch a 0 
 digitalWrite(25,HIGH);//met le commun 2 des rotary switch a 1 
 selected_mode = ((PINA >>2)&~B110000);
 }
 
 */




