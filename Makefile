# Platform conf
# Avrdude conf (flash board)
AVRDUDE      = /usr/bin/avrdude
AVRDUDE_CONF = /etc/avrdude.conf

# Tools used
CC=avr-gcc
CPP=avr-g++
AR=avr-ar
SIZE=avr-size
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump

# Build flags
CC_FLAGS=-MMD -DF_CPU=16000000L -mmcu=atmega328p\
	-DARDUINO=105\
	-D__PROG_TYPES_COMPAT__\
	-Os -std=gnu11 -flto -fno-fat-lto-objects\
	-Wall -ffunction-sections -fdata-sections

CPP_FLAGS=-MMD -DF_CPU=16000000L -mmcu=atmega328p\
  	-DARDUINO=105\
	-D__PROG_TYPES_COMPAT__\
	-Os -std=gnu++11 -flto -fno-threadsafe-statics\
	-fpermissive -fno-exceptions\
	-Wall -ffunction-sections -fdata-sections

# Link flags
LDFLAGS = -Os
LDFLAGS += -flto -fuse-linker-plugin
LDFLAGS += -Wl,--gc-sections

# Include path
INCLUDE = -I/usr/lib/avr/include
INCLUDE += -I/usr/share/arduino/hardware/arduino/variants/standard
INCLUDE += -I./arduino
INCLUDE += -I.

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

$(TARGET): $(OBJECTS) ./arduino/libarduino.a
	$(CC) -mmcu=atmega328p $(LIBPATH) $(LDFLAGS) -o $(TARGET) $(OBJECTS) arduino/libarduino.a -lc -lm
	$(OBJCOPY) -j .eeprom --set-section-flags=.eeprom='alloc,load' \
		--no-change-warnings --change-section-lma .eeprom=0 -O ihex $(TARGET) $(TARGET).eep

%.o: %.c
	$(CPP) -c $(INCLUDE) $(CPP_FLAGS) $< -o $@

lst:  $(TARGET).lst

%.lst: $(TARGET)
	$(OBJDUMP) -h -S $< > $@

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
	$(AVRDUDE) -C $(AVRDUDE_CONF) -q -D -V -c arduino -p atmega328p -P /dev/ttyACM0 -b 115200 -U flash:w:$(TARGET).hex:i
