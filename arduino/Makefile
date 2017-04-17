# Tools used
CC=avr-gcc
CPP=avr-g++
AR=avr-gcc-ar

#Include paths
INC= -I/usr/share/arduino/hardware/arduino/variants/standard\
	  -I.

#Build flags
CPP_FLAGS=-MMD -DF_CPU=16000000L -mmcu=atmega328p\
	-DARDUINO=105\
	-D__PROG_TYPES_COMPAT__\
	-Os -std=gnu++11 -flto -fno-threadsafe-statics\
	-fpermissive -fno-exceptions\
	-Wall -ffunction-sections -fdata-sections

CC_FLAGS=-MMD -DF_CPU=16000000L -mmcu=atmega328p\
	-DARDUINO=105\
	-D__PROG_TYPES_COMPAT__\
	-Os -std=gnu11 -flto -fno-fat-lto-objects\
	-Wall -ffunction-sections -fdata-sections

C_SOURCES = $(wildcard *.c)
AVRLIBC_SOURCES = $(wildcard avr-libc/*.c)
CPP_SOURCES := $(filter-out main.cpp, $(wildcard *.cpp))
C_OBJECTS = $(C_SOURCES:.c=.c.o)
AVRLIBC_OBJECTS = $(AVRLIBC_SOURCES:.c=.c.o)
CPP_OBJECTS = $(CPP_SOURCES:.cpp=.cpp.o)

%.c.o: %.c
	$(CC) $(INC) $(CC_FLAGS) -c $< -o $@

avr-libc/%.c.o: avr-libc/%.c
	$(CC) $(INC) $(CC_FLAGS) -c $< -o $@

%.cpp.o: %.cpp
	$(CPP) $(INC) $(CPP_FLAGS) -c $< -o $@

libarduino: $(CPP_OBJECTS) $(C_OBJECTS) $(AVRLIBC_OBJECTS)
	$(AR) rcs libarduino.a $(CPP_OBJECTS) $(C_OBJECTS) $(AVRLIBC_OBJECTS)

.PHONY: clean
clean:
	rm -f libarduino.a *.o *.d
