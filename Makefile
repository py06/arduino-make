# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile
ARDUINO_DIR = /usr/share/arduino
ARDMK_DIR   = /usr/share/arduino
INCLUDE = -I/usr/lib/avr/include
INCLUDE += -I/usr/share/arduino/hardware/arduino/variants/standard
INCLUDE += -I./arduino
INCLUDE += -I.

LIBRARIES = -L/usr/lib/avr -L./arduino

#AVR_TOOLS_DIR = /usr

# Platform conf
BOARD_TAG  = uno #don't select atmega328 (it tries to uppload at 57600bps)
MONITOR_PORT  = /dev/ttyACM0
ISP_PORT = /dev/ttyACM*

# Avrdude conf (flash board)
AVRDUDE      = /usr/bin/avrdude
AVRDUDE_CONF = /etc/avrdude.conf

DEFINES=-DARDUINO_MAIN -D__AVR_ATmega328P__
CC=avr-gcc
CPP=avr-g++
AR=avr-ar
OBJCOPY=avr-objcopy
CC_FLAGS=-D__AVR_ATMEGA328P__ -Os -DF_CPU=16000000UL -mmcu=atmega328p\
	-DARDUINO=105 -DARDUINO_ARCH_AVR\
	-D__AVR_ATmega328P__1 -DBOARD_TAG=uno\
	-D__PROG_TYPES_COMPAT__\
	-Wall -ffunction-sections -fdata-sections

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
TARGET = thermometer

$(TARGET).hex: $(TARGET)
	$(OBJCOPY) -O ihex -R .eeprom  $(TARGET) $(TARGET).hex

$(TARGET): $(OBJECTS)
	$(CC) -mmcu=atmega328p $(LIBRARIES) $(OBJECTS) -larduino -o $(TARGET)

%.o: %.c
	$(CPP) -c $(INCLUDE) $(CC_FLAGS) $< -o $@

clean:
	rm -f $(TARGET).hex $(TARGET) $(OBJECTS)


upload: $(TARGET).hex
	$(AVRDUDE) -C /etc/avrdude.conf -q -D -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:$(TARGET).hex:i

# avr-gcc -Os -DF_CPU=16000000UL -mmcu=atmega328p -c -o led.o led.c
# avr-gcc -mmcu=atmega328p led.o -o led
# avr-objcopy -O ihex -R .eeprom led led.hex
# avrdude -F -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:led.hex

# !!! Important. You have to use make ispload to upload when using ISP programmer
