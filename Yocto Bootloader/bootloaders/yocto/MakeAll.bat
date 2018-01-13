echo off
cd C:\arduino-1.0.5\hardware\Yocto\bootloaders\yocto
call make clean
call make -f Makefile
copy BootYocto.hex c:\tools
pause