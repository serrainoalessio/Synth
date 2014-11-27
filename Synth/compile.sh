COMPILE_CMD="avr-gcc -O2 -mmcu=atmega328p -Wall -o ./synth.out ./synth.c"
GENHEX_CMD="avr-objcopy -O ihex -R .eeprom synth.out synth.hex"
AVRDUDE_CMD="avrdude -C /Users/alessioserraino/Downloads/GCC-10.6/tools/avr/etc/avrdude.conf -p m328p -c stk500v1 -P /dev/tty.usbmodem641 -b 19200 -e -U flash:w:./synth.hex"

#step1
printf "\033[33;49m"
echo "===== COMPILING ====="
printf "\033[36;49m"
echo " > executing: " $COMPILE_CMD
$COMPILE_CMD >& /dev/null
if [ $? -eq 0 ] #eq = equal, ne = not equal
  then
    printf "\033[32;49m"
    echo " > copmilation went ok"
  else
    printf "\033[31;49m"
    echo " > compialtion failed with stat " $0
    echo " > stopping"
    printf "\033[39;49m"
    exit
fi

#step2
printf "\033[33;49m"
echo "===== GENERATING HEX ====="
printf "\033[36;49m"
echo " > executing: " $GENHEX_CMD
$GENHEX_CMD >& /dev/null
if [ $? -eq 0 ] #eq = equal, ne = not equal
  then
    printf "\033[32;49m"
    echo " > objcopy went ok"
  else
    printf "\033[31;49m"
    echo " > objcopy failed with stat " $0
    echo " > stopping"
    printf "\033[39;49m"
    exit
fi

#step 3
printf "\033[33;49m"
echo "===== PUSHING PROGRAM ====="
printf "\033[36;49m"
echo " > executing: " $AVRDUDE_CMD
$AVRDUDE_CMD >& /dev/null
if [ $? -eq 0 ] #eq = equal, ne = not equal
  then
    printf "\033[32;49m"
    echo " > avrdude went ok"
  else
    printf "\033[31;49m"
    echo " > avrdude failed with stat " $0
    echo " > stopping"
    printf "\033[39;49m"
    exit
fi
printf "\033[39;49m"
