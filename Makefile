export USER_LIB_PATH = ${shell pwd}/Library
ARDMK = make -f ${ARDMK_DIR}/Arduino.mk
OUTPUT_DIR = build-${BOARD_TAG}

default:	yocto.ino
	${ARDMK} clean
	${ARDMK}
	srec_cat ${OUTPUT_DIR}/yocto.hex -Intel -Output -Binary | bin/yocto-bin2syx > ${OUTPUT_DIR}/yocto.syx

# Glue all the .ino files together, or Arduino.mk won't work.
# yocto.ino is phony to avoid Docker clock skew weirdness
.PHONY:	yocto.ino
yocto.ino:
	echo "#include \"${shell pwd}/yocto.h\"" > $@
	bin/cat-first Yocto/Yocto*.ino Yocto/*.ino >> $@
