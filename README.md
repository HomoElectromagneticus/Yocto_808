# Yocto_808
Unofficial firmware for the DIY [TR-808 Clone by E-Licktronic](http://www.e-licktronic.com/en/content/25-yocto-tr808-clone-tr-808).

## For users:
Click the [releases](https://github.com/HomoElectromagneticus/Yocto_808/releases) tab above to download the latest release.

Read the [improved Yocto manual](https://github.com/HomoElectromagneticus/Yocto_808/blob/master/Manual/manual.adoc).

Update the Yocto firmware by holding step buttons 1, 3, and 5 while powering the unit on. Start sending the SysEx to the Yocto's MIDI IN port within five seconds. Be sure to put a delay between SysEx messages of ~200ms, or the Yocto may not take the update. Use of MIDI-OX on Windows and SysExLibrarian on Mac is recommended.

If there are issues, don't hesitate to bring them up on the [e-licktronic forum](http://www.e-licktronic.com/forum/viewforum.php?f=17).

## For developers:
### Burning the bootloader onto the Yocto's microprocessor (Using Arduino IDE v1.0.6)
A useful guide to setup an Arduino as an In-System Programmer is available at the e-licktronic forum (http://www.e-licktronic.com/forum/viewtopic.php?f=17&t=436)

### Writing firmware to the Yocto's microprocessor with an Arduino
1. Find out what micro you have - ATMEGA1284 or ATMEGA1284P? If you have the ATMEGA1284, 
   update your avrdude.conf file with what's in the "avrdude.conf.addendum" file.
2. Burn the bootloader following the above guide from e-licktronic.
3. Make sure the "TX" connection on the Arduino is not connected to anything.
4. Make sure that GND and RST are not connected.
5. Make sure your libraries are in the right place or your code won't compile.
6. Program / Upload / Burn. If the process fails, you may need to burn the bootloader again.

### Compiling a .hex and .syx file with Jacob Vosmaer's build tool
1. Visit https://gitlab.com/jacobvosmaer/yocto-build and do what it says.

### Reading serial output from the Yocto's microprocessor with an Arduino
1. Make the RX / TX connections given in the "Hardware_Connections.png" image.
2. Connect GND and RST on the Arduino board.
3. Open your favorite serial monitor software and monitor the COM point.
4. Set the software to 115200 baud.

### Hey, playing with the firmware screwed up the patterns on the CB, CY, OH, CH...
You probably didn't increase the buffer size in the Arduino twi and Wire libraries. 
In twi.h:
  "#define TWI_BUFFER_LENGTH 128" instead of 32 
In Wire.h:
  "#define BUFFER_LENGTH 128" instead of 32
See (http://www.e-licktronic.com/forum/viewtopic.php?f=17&t=180&p=3716&hilit=libraries#p3716)

### Disclaimer
I am not affiliated in any way with e-licktronic. Use this code at your own risk!
