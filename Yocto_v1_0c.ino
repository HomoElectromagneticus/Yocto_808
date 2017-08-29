
/* Yocto clone TR808 Sequencer Firmware
*  
*  v1.0c: -Fix BUG synchro Midi Slave
*
*  v1.0b: -Ajout du mode Solo en restant appuyer sur le bouton mute quand on est dans le mode mute
*         -Fix BUG du trig CPU
*
*  v1.0a: -Fix les sorties TRIG en S-TRIG
*         -Améliore BUG de la fonction ROLL
*
*  v1.0:  -Programme de base
*/


#include <TR_SEQ.h>
#include <EEPROM.h>
#include <Wire.h>
#include <TimerOne.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <MIDI.h>

#define BD_MIDI_NOTE 36  //C2
#define SD_MIDI_NOTE 38  //D2
#define LT_MIDI_NOTE 41  //F2
#define MT_MIDI_NOTE 43  //G2
#define HT_MIDI_NOTE 47  //B2
#define RM_MIDI_NOTE 37  //C#2
#define HC_MIDI_NOTE 39  //D#2
#define CW_MIDI_NOTE 56  //G#3
#define CY_MIDI_NOTE 49  //C#3
#define OH_MIDI_NOTE 46  //A#2
#define CH_MIDI_NOTE 42  //F#2

#define DEBUGG 0 // permet de debugger le programme via le moniteur serie d'arduino
#define DEBOUNCE 5 // debounce pour eviter les rebonds des boutons

#define NBR_INST 16 //nombre d'instrument max
#define NBR_PTRN 16 //nombre de pattern par banque
#define MAX_PATTERN_SONG 255 //nombre max de pattern par song soit 256 parce qu'on compte le 0

#define TRIG1_PIN 14// attention Trig 3 sur le schema correspond a Trig 1 sur les sortie de la Yocto et vis et versa
#define TRIG2_PIN 1
#define TRIG3_PIN 0
#define TRIG_CPU_PIN 2

#define MASTER 0
#define DIN_SLAVE 1
#define MIDI_SLAVE 2

// Version pour pouvoir jouer la jouer dans init Epp
byte major_version = 2;
byte minor_version = 0;


//ici se trouve toutes les variables du programme
//------------------------------------------------

byte selected_inst; //variable de l'instrument selectioner cf Check_rotary_switch

//Variable des modes
//====================================================
byte selected_mode; //variable du mode selectionner 
byte old_selected_mode=17;/*variable qui permet d'indiquer qu'on a change de mode initialise a un mode qui n'existe pas pour pouvoir rentree dans n'importe que
 mode*/
boolean first_time_clear_pattern=0;//flag que l'on vient de rentrée dans le mode clear pattern
boolean first_time_copy_pattern=0;//flag que l'on vient de rentrée dans le mode copy pattern
boolean first_time_paste_pattern=0;//flag que l'on vient de rentrée dans le mode paste pattern

unsigned int bpm; // bpm du sequenceur depend de l'encodeur
unsigned int timer_time;// temps d'overflow du timer depend du bpm
byte sync_mode;//mode de synhcronisation 0=master 1=Din slave 2=Midi slave

//variable qui serve a checker les buttons Edits
//====================================================
boolean play =0;//variable de l'etat du bouton play en volatile car elle peut etre modifier dans l'interruption et dans le programme
//--------------------utiliser dans les interruptions-------------------------------//
volatile boolean first_play=0;//Utiliser pour lire le premier pas  indique que c la priere boucle apres l'appuie sur play
volatile boolean first_play_A=0;//idem premiere
volatile boolean first_play_B=0;//utiliser dans l'interuption pour ne pas switcher le premier load en mode song play
boolean first_play_C=0;//idem utiliser pour lire le premier pattern dans un block
volatile boolean first_stop=0;//uriliser pour initaliser UNE FOIS apres un stop comme envoyer les messages MIDI STOP
//----------------------------------------------------------------------------------
byte button_play_count =0;//compteur du nombre d'appuie sur le bouton play
boolean button_play=0;//etat du bouton play
byte edit_button_state, old_edit_button_state, debounce_edit_button_state;//etat des boutons edit
boolean button_pattern_part=0;//etat des deux boutons de selections de la parti du pattern 0 celui de gauche 1 celui de droite
byte button_scale_count=0;//compte le nombre d'appuie sur le boutons scale de 0 a 3
boolean button_shift=0;// etat du bouton shift//variable qui serve a checker les buttons Edits
boolean button_encoder=0;//etat du bouton de l'encoder
boolean button_init=0;//variable qui correspond au bouton play et shift 
boolean button_next=0;//bouton next en mode song
boolean first_push_next=0;//flag de l'etat du permier appuie sur next
boolean button_reset=0;//bouton reset en mode song
boolean button_end=0;//bouton end en mode song
boolean first_push_end=0;//flag de l'etat du premier appuie sur end
boolean  old_din_start_state=2;//variable qui sert achecker le changement de statut de start en DIN_SYNC initialise a 2 pour que le fonction s'effectuer au demarrag

boolean button_part_switch=1;//variable des deux boutons part appuyer initialise a 1 comme ça au demmarrage les pattern switch automatiquement

//variable qui serve a checker les boutons steps
//====================================================
int step_button_state, old_step_button_state, debounce_step_button_state;//etat des boutons step

//Variable du pattern
//====================================================
volatile boolean pattern_buffer=0;//permet de passer du pattern selectionner au prochain pattern en creant un buffer
unsigned int pattern[2][NBR_INST][2];//un pattern est constitue de 16 instruments fois 2 pour les pas de 1a16 et de 17a32 et de deux buffer d'ou le premier array avec 2
unsigned int copy_pattern_buffer[NBR_INST][2];// buffer pour copier un pattern
byte copy_pattern_nbr_step=0;//Buffer du nombre de step du pattern a copier
byte copy_pattern_scale=0;//Buffer de la scale du pattern a copie
byte selected_pattern=0;//variable du pattern selectionner de 0 a 15
byte old_pattern_nbr=1;/*variable du prochain pattern selectionner doit etre different de selected_pattern a l'initialisation pour pouvoir loader le premeir
 pattern*/
byte pattern_nbr=0;//numero du pattern de 0 a 255
byte next_pattern_nbr=0;//utiliser dans le mode song play pour charger le pattern suivant dans le buffer
boolean pattern_clear_ok=0;//flag que le pattern a bien etait clearer
boolean pattern_copy_ok=0;//le pattern selectionner n'a pas été copier
volatile boolean selected_pattern_changed=1;//indique si le pattern selectionner a  change
boolean selected_pattern_changed_A=0;//indique si le pattern selectionner a  change
boolean selected_pattern_edited=0;//indique que le pattern a ete edite
boolean selected_pattern_edited_saved=0;//indique que le pattern a ete edite et sauver
unsigned int inst_step_buffer[32][2]={
  0};//Array de l'etat de chaque pas du sequenceur *2 pour deux buffer
byte nbr_step[2];//nombre de step par pattern va de 1 à 32 ,une array de deux pour le buffer 
byte pattern_scale[2];//Scale de chaque pattern, array de deux pour le buffer 
boolean nbr_step_changed=0;//flag que le nbr de pas du pattern a change
boolean pattern_scale_changed=0;//flag que la scale du pattern a changer
byte pattern_bank=0;//variable de la bank selectionner
boolean load_pattern_ok=0;//flag que le pattern a bien ete loader

//Variable qui servent au block
byte pushed_button_step_count=0;//compte le nombre de bouton step appuyer simultanement
byte first_selected_pattern=0;//premier pattern selectionner sert lors de l'appuie sur plusieur bouton pour selectionner un block
byte last_selected_pattern=0;//dernier pattern selectionner
byte low_selected_pattern=0;//retiens le patternle plus petit du block
byte nbr_pattern_block=0;//Nombre de pattern dans le block
boolean nbr_pattern_block_changed=0;//flag que le nombre de block a change
volatile boolean nbr_pattern_block_changed_A=0;//flag que le nombre de block a change utiliser dans l'interuption

volatile byte pattern_count=0;//utiliser dans les interruption pour faire avance les pattern en mode song ou quand un block de pattern est selectionner

//Variable du song
//====================================================
byte selected_song=0;//numero du song selectionner va de 0 a 15
byte old_selected_song=20;/*different du selected song pour loader le song au setup et different de 0a15 au cas ou la postion du rotary soit egal a old_selecte
 pattern*/
byte next_selected_song=1;//different du selected song pour l'initialisation, sert a savoir si on a changer de song sur le rotary swtich
byte song[256][2];//un song est composer de 256 patterns au maximum, il enregistre le numéro des pattern a chainer *2 pour le buffer
volatile byte nbr_pattern_song=0;//Compteur du numero de la position du pattern dans le song (cf interrupt)
byte old_nbr_pattern_song=10;//utiliser pour marquer le changement de pattern en mode song play
byte total_pattern_song[2]={
  255,255};//Nombre de pattern total du song *2 pour le buffer
byte song_buffer=0;//flag du buffer du song selectionner
boolean selected_song_changed=0;//flag que le song selectionne a changer
// boolean selected_song_changed_A=0;//flag que le song selectionne a changer
boolean selected_song_edited=0;//flag que la song a ete editer
boolean load_song_ok=0;//flag que le song a ete loader dans le buffer

//variable du timer et de l interuption sur PCINT3
//====================================================
boolean tempo_led_flag=0;//flag de la led qui clignote suivant le tempo. Depend si PLay ou pas
boolean tempo_led_flag_block=0;//flag qui fait clignoter la led du pattern selectionner au tempo quand un block est selectionner en play
byte tempo_led_count=0;//incrementer dans le timer a 96ppqn 
byte ppqn_count=0;//compte 24 fois par pas
byte step_count=0;//compteur de pas
boolean step_changed=0;//flag que le pas a change
byte debut_mesure_count=0;//utiliser pour toujours savoir quand la mesure recommencer tous les 16pas
volatile boolean end_mesure_flag=0;//indique qu'on est passe a la mesure suivante, utiliser dans le mode song play pour faire incrementer le compteur 
volatile boolean middle_mesure_flag=0;//utiliser pour avancer au pattern suivant lors de la selection d'un block indique le milieu de la mesure

//Variable du DEBOUNCE
unsigned long millis_debounce_step_button, millis_debounce_edit_button=0;//variable pour le temps de debounce des boutons

//Variable des step leds
unsigned int temp_step_led;//valeur des led de step

//Variable du mute mode et roll mode
boolean roll_mode=0;//statut du roll mode
boolean mute_mode=0;//variable qui indique si on est ou pas dans le mute mode
boolean solo_mode=0;//solo mode 1 =active 0=desactive
unsigned int inst_roll=0;//retiens si un instrument et roller ou pas sur un mot de 16bit chaque bit correspond au mute de chaque instruments
byte roll_pointer=0;//incementer par l'encoder en mode roll
byte roll_scale[2][4]={
  { 
    24,12,6,3      }
  ,{
    32,16,8,4      }
};//24 correspond a un roulement de 1/16 etc... deux array pour le mode ternaire
byte scale_type=0;//
byte temp_roll_scale=2;//
byte step_button_just_pressed[16], step_button_pressed[16], step_button_current_state[16], step_button_previous_state[16];//variable qui servent a la fonction mute pour pouvoir appuyer sur plusieur bouton en meme temps
byte step_button_count[16]={
  0};//compte le nombre de fois qu'on appuie sur chaque bouton step
unsigned int inst_mute=0;//retiens si un instrument et muter ou pas sur un mot de 16bit chaque bit correspond au mute de chaque instruments


//Variable du mode MIDI PLAY
//====================================================
unsigned int inst_midi_buffer=0;
unsigned int old_inst_midi_buffer=0;
boolean trigg_ok=0;//indique que les inst peuvent etre trigger
volatile byte flag_note_on=0;//flag que l'on est rentrer dans l'interrupt noteON
volatile unsigned int count=0;//idem premier compteur
byte selected_channel;//channel MIDI In
boolean selected_channel_changed=0;//flag que le channel selectionner a changer
int inst_midi_note[16];//Array des note midi de chaque instruments
boolean inst_midi_note_edited=0;//flag que les note midi ont changé


unsigned long timer_off=0;

unsigned long noteOnOff[16];

void defNotes()
{
  inst_midi_note[0]= 59; //Array des note midi de chaque instruments
  inst_midi_note[1]= BD_MIDI_NOTE;//Array des note midi de chaque instruments
  inst_midi_note[2]= SD_MIDI_NOTE;//Array des note midi de chaque instruments
  inst_midi_note[3]= LT_MIDI_NOTE;//Array des note midi de chaque instruments
  inst_midi_note[4]= MT_MIDI_NOTE;//Array des note midi de chaque instruments
  inst_midi_note[5]= HT_MIDI_NOTE;//Array des note midi de chaque instruments
  inst_midi_note[6]= RM_MIDI_NOTE;//Array des note midi de chaque instruments
  inst_midi_note[7]= HC_MIDI_NOTE;//Array des note midi de chaque instruments
  inst_midi_note[8]= CW_MIDI_NOTE;//Array des note midi de chaque instruments
  inst_midi_note[9]= CY_MIDI_NOTE;//Array des note midi de chaque instruments
  inst_midi_note[10]=OH_MIDI_NOTE;//Array des note midi de chaque instruments
  inst_midi_note[11]=CH_MIDI_NOTE;//Array des note midi de chaque instruments
  inst_midi_note[12]=60; //Array des note midi de chaque instruments
  inst_midi_note[13]=62; //Array des note midi de chaque instruments
  inst_midi_note[14]=64; //Array des note midi de chaque instruments
  inst_midi_note[15]=65; //Array des note midi de chaque instruments
}
//====================================================
//debut du setup
//====================================================
void setup(){

  /* //Initialise l'UART1
   UCSR1B |= (1 << RXEN1) | (1 << TXEN1);   // Active la transmission et la reception sur l'UART1
   UCSR1C |= (1 << UCSZ10) | (1 << UCSZ11); // Use 8-bit character sizes
   UBRR1H = (BAUD_PRESCALE >> 8); // Load upper 8-bits of the baud rate value into the high byte of the UBRR register
   UBRR1L = BAUD_PRESCALE; // Load lower 8-bits of the baud rate value into the low byte of the UBRR register*/

  //Initialise les entrees sorties
  DDRA |= ~B11111100;// les PIN 39 et 40 sont en sorti pour le commun des rotary switch les autres sont en entree pour l'encoder et les rotary switch
  DDRC |= B11111100;// les 6 bits fort du PORTC sont en sorti pour les 6 led Edit
  //DDRB |= B00000111;// les trois sorties TRIG  CPU,1 et 2
  DDRD |= (1<<6);//sortie trig out

  pinMode(TRIG1_PIN,OUTPUT);
  pinMode(TRIG2_PIN,OUTPUT);
  pinMode(TRIG3_PIN,OUTPUT);
  pinMode(TRIG_CPU_PIN,OUTPUT);

  //initialise les parametre du sequenceur
  bpm=480;//BPM reel = bpm/4
  timer_time =((unsigned int)(2500000/bpm));
  Timer1.initialize(timer_time); // set a timer of length in microseconds 

  //Initialise les liaisons serie
  Serial.begin(115200);//liaison  serie pour debugger

  MIDI.begin(MIDI_CHANNEL_OMNI); 

  defNotes();

  //Initialise les bibliothèques
  SR.Initialize();
  Wire.begin();

  memset(pattern,0,sizeof(pattern));//initialise l'array du pattern a 0.
  memset(song,0,sizeof(song));//initialise l'array du song a 0.

  Load_Pattern();//on load le pattern 0
}



void Chenillard()
{
  int temp=0;//variable pour le chenillard
  for (byte i=0;i<16;i++)
  {
    temp |=1<<i;
    SR.Led_Step_Write(temp);
    delay(30);
  }
  SR.Led_Step_Write(0);//initialise les led a 0 apres le chenillard
}






