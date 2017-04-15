# Arduino Make file. Refer to https://github.com/sudar/Arduino-Makefile
ARDUINO_DIR = /usr/share/arduino
ARDMK_DIR   = /usr/share/arduino
#AVR_TOOLS_DIR = /usr

# Platform conf
BOARD_TAG  = uno #don't select atmega328 (it tries to uppload at 57600bps)
MONITOR_PORT  = /dev/ttyACM0
ISP_PORT = /dev/ttyACM*

# Avrdude conf (flash board)
AVRDUDE      = /usr/bin/avrdude
AVRDUDE_CONF = /etc/avrdude.conf

ARCHITECTURE = avr
ARDMK_VENDOR = arduino
CURRENT_OS = LINUX
#MCU = atmega328p
#CORE = arduino
#VARIANT = mega
#ARDUINO_VERSION = 105
CC=avr-gcc
CPP=avr-g++
INC = -I/usr/lib/avr/include\
	  -I/usr/share/arduino/hardware/arduino/variants/standard\
	  -I.

LIBRARIES = -L/usr/lib/avr

CPP_FLAGS=-MMD -DF_CPU=16000000UL -mmcu=atmega328p\
	-DARDUINO=105\
	-D__PROG_TYPES_COMPAT__\
	-Os -std=gnu++11 -flto -fno-threadsafe-statics\
	-fpermissive -fno-exceptions\
	-Wall -ffunction-sections -fdata-sections

CC_FLAGS=-MMD -DF_CPU=16000000UL -mmcu=atmega328p\
	-DARDUINO=105\
	-D__PROG_TYPES_COMPAT__\
	-Os -std=gnu11 -flto -fno-fat-lto-objects\
	-Wall -ffunction-sections -fdata-sections
AR=avr-gcc-ar

C_SOURCES = $(wildcard *.c)
CPP_SOURCES := $(filter-out main.cpp, $(wildcard *.cpp))
C_OBJECTS = $(C_SOURCES:.c=.c.o)
CPP_OBJECTS = $(CPP_SOURCES:.cpp=.cpp.o)

%.c.o: %.c
	@echo "$< $@ INC=$(INC)"
	$(CC) $(INC) $(CC_FLAGS) -c $< -o $@

%.cpp.o: %.cpp
	@echo "$< $@ INC=$(INC)"
	$(CPP) $(INC) $(CPP_FLAGS) -c $< -o $@

libarduino: $(CPP_OBJECTS) $(C_OBJECTS)
	$(AR) rcs libarduino.a $(CPP_OBJECTS) $(C_OBJECTS)

.PHONY: clean
clean:
	rm -f libarduino.a *.o *.d
