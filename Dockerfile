FROM ubuntu:16.04

RUN apt-get update

RUN apt-get install -y \
  arduino-mk \
  ruby2.3 \
  srecord \
  git

ENV YOCTO_HARDWARE /mighty-1284p
RUN git clone https://github.com/maniacbug/mighty-1284p $YOCTO_HARDWARE
RUN cd $YOCTO_HARDWARE && git checkout 68ed99c0e9f1c130a7ac8bbd54ea80634cb11802

# Patches for correct EEPROM access
RUN echo '#define BUFFER_LENGTH 128' >> /usr/share/arduino/libraries/Wire/Wire.h
RUN echo '#define BUFFER_LENGTH 128' >> /usr/share/arduino/libraries/Wire/utility/twi.h
RUN echo '#define TWI_BUFFER_LENGTH 128' >> /usr/share/arduino/libraries/Wire/utility/twi.h

ENV ARDUINO_DIR /usr/share/arduino
ENV ARDMK_DIR /usr/share/arduino
ENV AVR_TOOLS_DIR /usr
ENV BOARD_TAG mighty
ENV BOARDS_TXT $YOCTO_HARDWARE/boards.txt
ENV ARDUINO_VAR_PATH $YOCTO_HARDWARE/variants
ENV ARDUINO_CORE_PATH $YOCTO_HARDWARE/cores/standard
ENV ALTERNATE_CORE standard


WORKDIR /yocto

CMD ["make"]
