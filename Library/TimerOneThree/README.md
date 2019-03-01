TimerOneThree
=============

The arduino TimerOne library adapted for ATmega1284P and extended for timer3

The arduino TimerOne library described at http://playground.arduino.cc/Code/Timer1 is the starting point for this lib. The original library can be found at http://code.google.com/p/arduino-timerone/downloads/list.

The commands are the same as in the TimerOne library. The pinmapping is taken from maniacbug's mighty ATmega1284P. http://maniacbug.wordpress.com/2011/11/27/arduino-on-atmega1284p-4/
* pin 6 is Timer3A
* pin 7 is Timer3B
* pin 13 is Timer1A
* pin 12 is Timer1B

### Installation and usage

To install simply create a folder named TimerOneThree in your libraries folder of the arduino IDE. Download the zip archive. Copy all files from the zip archive into the TimerOneThree folger. Then restart the IDE. In your sketch type:  
`#include <TimerOneThree.h>`

With the TimerOneThree lib both timer1 and timer3 of the ATmega1284P can be used in the same way, e. g:

`Timer1.initialize(2000); // sets timer1 to a period of 2000 microseconds`  
`Timer3.initialize(2000); // does the same for timer3`

So when you want to set use timer3A to generate a pwm with 50% duty cycle:

`Timer3.pwm(6, 511);  // duty cycle goes from 0 to 1023`
