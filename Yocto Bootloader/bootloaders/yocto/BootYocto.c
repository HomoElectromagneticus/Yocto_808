/**********************************************************/
/* Midi Bootloader for Atmel megaAVR Controllers          */
/* Pour Yocto                                             */
/* Laurent Lecatelier                                     */
/**********************************************************/

/* some includes */
#include <inttypes.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include <avr/eeprom.h>
#include <avr/boot.h>

#define MAX_ERROR_COUNT 5
#define NUM_LED_FLASHES 2


#ifndef BAUD_RATE
#define BAUD_RATE   31250
#endif


/* SW_MAJOR and MINOR needs to be updated from time to time to avoid warning message from AVR Studio */
/* never allow AVR Studio to do an update !!!! */
#define HW_VER	 0x01
#define SW_MAJOR 0x01
#define SW_MINOR 0x10



#define LED_DDR  DDRB
#define LED_PORT PORTB
#define LED_PIN  PINB
#define LED      PINB0


/* define various device id's */
/* manufacturer byte is always the same */
#define SIG1	0x1E	// Yep, Atmel is the only manufacturer of AVR micros.  Single source :(
#define SIG2	0x97
#define SIG3	0x05


#define MATCHING_HEADER 0
#define READING_COMMAND 2
#define READING_DATA 3
  
  
 
  
/* function prototypes */
void putch(char);
char getch(void);
void flash_led(uint8_t);
void shiftOut_SRIO(uint8_t value);
uint8_t shiftIn_SRIO(void) ;
uint8_t Button_Step_Read(void) ;
void Midiloop(void);
void LedStep(uint16_t value);
void boot_program_page (uint32_t page, uint8_t *buf);


/* some variables */
union address_union
{
  uint16_t word;
  uint8_t  byte[2];
} address;

union length_union
{
  uint16_t word;
  uint8_t  byte[2];
} length;

struct flags_struct
{
  unsigned eeprom : 1;
  unsigned rampz  : 1;
} flags;


const uint8_t sysex_header[] = {
  0xf0,  // <SysEx>
  0x7d,  // Non commercial
  0x08, 0x08,  // Product ID  Yocto 0808
  0x02, // Nb Oct Cmd
};


uint32_t page = 0;
uint8_t rx_buffer[ 2 * (SPM_PAGESIZE + 1)];


uint8_t buff[256];
uint8_t address_high;

uint8_t pagesz=0x80;

uint8_t i;
uint8_t bootuart = 0;

uint8_t error_count = 0;

void (*app_start)(void) = 0x00000;




/* main program starts here */
int main(void)
{
  uint8_t ch;

  ch = MCUSR;
  MCUSR = 0;
  WDTCSR |= _BV(WDCE) | _BV(WDE);
  WDTCSR = 0;
 
/* 
  if (! (ch &  _BV(EXTRF))) // if its a not an external reset...
      app_start();  // skip bootloader
 */
 
	bootuart = 2;

  // Programmation de Serial 1		

	UBRR1L = (uint8_t)(F_CPU/(BAUD_RATE*16L)-1);
	UBRR1H = (F_CPU/(BAUD_RATE*16L)-1) >> 8;
	UCSR1A = 0x00;
	UCSR1C = 0x06;
	UCSR1B = _BV(TXEN1)|_BV(RXEN1);

  // Port B Pour lire les touches et emettre les Led 
  
  DDRB =  0b10111111;

  // Test du flash des 16 boutons
  
  
 
  if ( Button_Step_Read() == 0b10101 ) // On appuie au boot sur le bouton 1+3+5
  {
		flash_led(NUM_LED_FLASHES);
		
//		LedStep(SPM_PAGESIZE);
//		_delay_ms(300);
	
		Midiloop();	
 
		flash_led(NUM_LED_FLASHES);

 }
	
  app_start();
  
  return 0;
}


void putch(char ch)
{
	while (!(UCSR1A & _BV(UDRE1)));
	UDR1 = ch;
}


char getch(void)
{
  uint32_t count = 0;
  while(!(UCSR1A & _BV(RXC1)))
  {
	  /* 20060803 DojoCorp:: Addon coming from the previous Bootloader*/
	  /* HACKME:: here is a good place to count times*/
	  count++;
	  if (count > MAX_TIME_COUNT)
		app_start();
  }
  return UDR1;
}



void flash_led(uint8_t count)
{
  while (count--)
  {
 	PORTB &= ~(1 << 4 ); 
	shiftOut_SRIO(0xFF);
    shiftOut_SRIO(0xFF);
    PORTB |= ( 1 << 4 ); 

    _delay_ms(100);
	
	PORTB &= ~(1 << 4 ); 
	shiftOut_SRIO(0);
    shiftOut_SRIO(0);
    PORTB |= ( 1 << 4 ); 
    
	_delay_ms(100);
 
  }
}


uint8_t shiftIn_SRIO(void) 
{
    uint8_t value = 0;
	uint8_t i = 0;
    while (i < 8 ) {
		value |= (  ( ( PINB & 0b01000000 ) >> 6 ) << (7-i) );//value |  (digitalRead(6) << (7-i));
		PORTB |= (1<<7);//digitalWrite(7, HIGH);
		PORTB &= ~(1<<7);// digitalWrite(7, LOW);
		i++;
    }
    return value;
}

void shiftOut_SRIO(uint8_t value)
{
	uint8_t i;
	uint8_t mask=0x80;

	for (i = 0; i < 8; i++)  
    {
		if ( ( value & mask ) == mask )
			PORTB |= (1<<5);
		else
			PORTB &= ~(1<<5);
		PORTB |=  (1<<7) ; 
		PORTB &= ~(1<<7) ; 
		mask>>=1;
	}

}

uint8_t Button_Step_Read(void)
{
  uint8_t firstbyte;
  PORTB |= (1<<3);
  firstbyte = shiftIn_SRIO();
  PORTB &= ~(1<<3);
  return firstbyte;
}  


void LedStep(uint16_t value)
{
	PORTB &= ~(1 << 4 ); 
    shiftOut_SRIO( value >> 8 );
	shiftOut_SRIO( value & 0xFF );
    PORTB |= ( 1 << 4 ); 
}


void boot_program_page (uint32_t page, uint8_t *buf)
{
	uint16_t i;
	uint8_t sreg;

	// Disable interrupts.

	sreg = SREG;
	cli();

	eeprom_busy_wait ();

	boot_page_erase (page);
	boot_spm_busy_wait ();      // Wait until the memory is erased.

	for (i=0; i<SPM_PAGESIZE; i+=2)
	{
		// Set up little-endian word.

		uint16_t w =  buf[i] | (buf[i + 1] << 8);
	
		boot_page_fill (page + i, w);
	}

	boot_page_write (page);     // Store buffer in flash page.
	boot_spm_busy_wait();       // Wait until the memory is written.

	// Reenable RWW-section again. We need this if we want to jump back
	// to the application after bootloading.

	boot_rww_enable ();

	// Re-enable interrupts (if they were ever enabled).

	SREG = sreg;
}

void Midiloop(void) {

  uint8_t byte;
  uint16_t bytes_read = 0;
  uint16_t rx_buffer_index =0;
  uint8_t state = MATCHING_HEADER;
  uint8_t checksum = 0;
  uint8_t sysex_commands[2];
  uint16_t progress_counter = 0;

  page = 0;

  
  while (1) {

    byte = getch();
    // In case we see a realtime message in the stream, safely ignore it.
    if (byte > 0xf0 && byte != 0xf7) {
      continue;
    }

    switch (state) {
      case MATCHING_HEADER:
        if (byte == sysex_header[bytes_read]) {
          ++bytes_read;
          if (bytes_read == sizeof(sysex_header)) {
            bytes_read = 0;
            state = READING_COMMAND;
          }
        } else {
          bytes_read = 0;
        }
        break;
        

      case READING_COMMAND:
        if (byte < 0x80) {
          sysex_commands[bytes_read++] = byte;
          if (bytes_read == 2) {
            bytes_read = 0;
            rx_buffer_index = 0;
            checksum = 0;
            state = READING_DATA;
             ++progress_counter;
            LedStep(progress_counter);
         }
        } else {
             state = MATCHING_HEADER;
             progress_counter=0;
             LedStep(progress_counter);
             bytes_read = 0;
        }
        break;

      case READING_DATA:
        if (byte < 0x80) {
          if (bytes_read & 1) {
            rx_buffer[rx_buffer_index] |= byte & 0xf;
            if (rx_buffer_index < SPM_PAGESIZE ) {
              checksum += rx_buffer[rx_buffer_index];
            }
            ++rx_buffer_index;
          } else {
            rx_buffer[rx_buffer_index] = (byte << 4);
          }
          ++bytes_read;
        } else if (byte == 0xf7) {
          if (sysex_commands[0] == 0x7f &&
              sysex_commands[1] == 0x00 &&
              bytes_read == 0) {
            // Reset.
            return;
          } else if (rx_buffer_index == SPM_PAGESIZE + 1 &&
                     sysex_commands[0] == 0x7e &&
                     sysex_commands[1] == 0x00 && 
                    rx_buffer[rx_buffer_index - 1] == checksum) {
            // Block write.
            boot_program_page(page,rx_buffer);
			page += SPM_PAGESIZE;
            ++progress_counter;
            LedStep(progress_counter);
          }
          state = MATCHING_HEADER;
          bytes_read = 0;
        }
        break;
    }
  }
}

/* end of file BootYocto.c */
