# Yocto_808
Firmware for a TR-808 Clone by E-Licktronic (http://www.e-licktronic.com/en/content/25-yocto-tr808-clone-tr-808)

# Burning the bootloader onto the Yocto's microprocessor
A useful guide to setup an Arduino as an In-System Programmer is available at the e-licktronic forum (http://www.e-licktronic.com/forum/viewtopic.php?f=17&t=436)

# Writing firmware to the Yocto's microprocessor with an Arduino
1. Update your avrdude.conf file with what's in the "avrdude.conf.addendum" file.
2. Burn the bootloader following the above guide from e-licktronic.
3. Make sure the "TX" connection on the Arduino is not connected to anything.
4. Make sure that GND and RST are not connected.
5. Make sure your libraries are in the right place or your code won't compile.
6. Program / Upload / Burn. If the process fails, you may need to burn the bootloader again.

# Reading serial output from the Yocto's microprocessor with an Arduino
1. Make the RX / TX connections given in the "Hardware_Connections.png" image.
2. Connect GND and RST on the Arduino board.
3. Open your favorite serial monitor software and monitor the COM point.
4. Set the software to 115200 baud.

# Disclaimer
I am not affiliated in any way with e-licktronic. Use this code at your own risk!
