
# The name of your project (used to name the compiled .hex file)
TARGET = main

# configurable options
OPTIONS = -DF_CPU=96000000 -DUSB_MIDI -DLAYOUT_US_ENGLISH

# options needed by many Arduino libraries to configure for Teensy 3.0
OPTIONS += -DARDUIO=105 -D__MK20DX256__ -DTEENSYDUINO=118


#************************************************************************
# Location of Teensyduino utilities, Toolchain, and Arduino Libraries.
# To use this makefile without Arduino, copy the resources from these
# locations and edit the pathnames.  The rest of Arduino is not needed.
#************************************************************************
# path location for Teensy Loader, teensy_post_compile and teensy_reboot
ARDUINOHOME = /home/john/arduino/
TOOLSPATH = $(ARDUINOHOME)/hardware/tools
#TOOLSPATH = ../../../tools/avr/bin   # on Mac or Windows

# path location for Arduino libraries (currently not used)
LIBRARYPATH = $(ARDUINOHOME)/libraries/

# path location for the arm-none-eabi compiler
COMPILERPATH = $(ARDUINOHOME)/hardware/tools/arm/bin

#************************************************************************
# Settings below this point usually do not need to be edited
#************************************************************************

# CPPFLAGS = compiler options for C and C++
OPTIMIZATION = 3
CPPFLAGS = -Wall -g  -mcpu=cortex-m4 -mthumb -nostdlib -MMD $(OPTIONS) -I. -I./stmlib/third_party/STM -O$(OPTIMIZATION) -J 4



# compiler options for C++ only
CXXFLAGS = -std=gnu++0x -felide-constructors -fno-exceptions -fno-rtti

# compiler options for C only
CFLAGS =

# linker options
LDFLAGS = -Os -Wl,--gc-sections -mcpu=cortex-m4 -mthumb -Tmk20dx256.ld

# additional libraries to link
LIBS = -lm


# names for the compiler programs
CC = $(COMPILERPATH)/arm-none-eabi-gcc
CXX = $(COMPILERPATH)/arm-none-eabi-g++
OBJCOPY = $(COMPILERPATH)/arm-none-eabi-objcopy
SIZE = $(COMPILERPATH)/arm-none-eabi-size

# automatically create lists of the sources and objects
# TODO: this does not handle Arduino libraries yet...
C_FILES := $(wildcard *.c)
CPP_FILES := $(wildcard *.cpp)
CC_FILES := $(wildcard *.cc)
OBJS := $(C_FILES:.c=.o) $(CPP_FILES:.cpp=.o) $(CC_FILES:.cc=.o)




# the actual makefile rules (all .o files built by GNU make's default implicit rules)

all: $(TARGET).hex

$(TARGET).elf: $(OBJS) mk20dx256.ld
	$(CC) $(LDFLAGS) -o $@ $(OBJS) $(LIBS) 

%.hex: %.elf
	$(SIZE) $<
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(TOOLSPATH)/teensy_post_compile -file=$(basename $@) -path=$(shell pwd) -tools=$(TOOLSPATH)
	-$(TOOLSPATH)/teensy_reboot


# compiler generated dependency info
-include $(OBJS:.o=.d)

clean:
	rm -f *.o *.d $(TARGET).elf $(TARGET).hex

upload:
	$(TOOLSPATH)/teensy_post_compile -file=$(TARGET) -path=$(shell pwd) -tools=$(TOOLSPATH)
	-$(TOOLSPATH)/teensy_reboot

#	teensy_loader_cli -mmcu=$(MCU) -s -w -v  $(TARGET).hex
	



