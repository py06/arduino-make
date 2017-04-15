# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile
ARDUINO_DIR = /usr/share/arduino
ARDMK_DIR   = /usr/share/arduino
INCLUDE = -I/usr/lib/avr/include
INCLUDE += -I/usr/share/arduino/hardware/arduino/variants/standard
INCLUDE += -I./arduino
INCLUDE += -I.

LIBPATH = -L/usr/lib/avr -L./arduino

#AVR_TOOLS_DIR = /usr

# Platform conf
BOARD_TAG  = uno #don't select atmega328 (it tries to uppload at 57600bps)
MONITOR_PORT  = /dev/ttyACM0
ISP_PORT = /dev/ttyACM*

# Avrdude conf (flash board)
AVRDUDE      = /usr/bin/avrdude
AVRDUDE_CONF = /etc/avrdude.conf

CC=avr-gcc
CPP=avr-g++
AR=avr-ar
SIZE=avr-size
OBJCOPY=avr-objcopy
CC_FLAGS=-MMD -DF_CPU=16000000UL -mmcu=atmega328p\
	-DARDUINO=105\
	-D__PROG_TYPES_COMPAT__\
	-fpermissive -fno-exceptions -flto\
	-std=gnu++11 -fno-threadsafe-statics\
	-Wall -ffunction-sections -fdata-sections

CPP_FLAGS=-MMD -DF_CPU=16000000UL -mmcu=atmega328p\
  	-DARDUINO=105\
	-D__PROG_TYPES_COMPAT__\
	-Os -std=gnu++11 -flto -fno-threadsafe-statics\
	-fpermissive -fno-exceptions\
	-Wall -ffunction-sections -fdata-sections

LDFLAGS = -Os -lc -lm -larduino
LDFLAGS += -flto -fuse-linker-plugin -larduino
LDFLAGS += -Wl,--gc-sections

SOURCES = $(wildcard *.c)
OBJECTS = $(SOURCES:.c=.o)
TARGET = thermometer

$(TARGET).hex: $(TARGET)
	$(OBJCOPY) -O ihex -R .eeprom  $(TARGET) $(TARGET).hex
	make size

.PHONY: libarduino
libarduino: ./arduino/libarduino.a

./arduino/libarduino.a:
	make -C arduino libarduino

$(TARGET): ./arduino/libarduino.a $(OBJECTS)
	$(CPP) -mmcu=atmega328p $(LIBPATH) $(OBJECTS) $(LDFLAGS) -o $(TARGET)

%.o: %.c
	$(CPP) -c $(INCLUDE) $(CC_FLAGS) $< -o $@

.PHONY: clean
clean:
	rm -f $(TARGET).hex $(TARGET) $(OBJECTS) $(ARDUINO_CPP_OBJ) $(ARDUINO_C_OBJ)

.PHONY: mrproper
mrproper: clean
	make -C arduino clean

.PHONY: size
size:
	$(SIZE) --mcu=atmega328p -C --format=avr $(TARGET)

upload: $(TARGET).hex
	$(AVRDUDE) -C /etc/avrdude.conf -q -D -V -c arduino -p ATMEGA328P -P /dev/ttyACM0 -b 115200 -U flash:w:$(TARGET).hex:i
