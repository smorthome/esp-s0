.PHONY: all clean image flash

# Application main file name
BIN = s0logger

# SDK and tools location
ESP_SDK = /home/cbdev/dev-ex/esp-open-sdk/sdk
EXTENSA_GCC = /home/cbdev/dev-ex/esp-open-sdk/xtensa-lx106-elf
ESPTOOL = ~cbdev/dev-ex/esptool/esptool.py
#ESP_DRIVER_LIB = $(ESP_SDK)/driver_lib
IMAGE1_SUFFIX =-0x00000.bin
IMAGE2_SUFFIX =-0x10000.bin

# Variable setup
export PATH := $(EXTENSA_GCC)/bin:$(PATH)
CC = xtensa-lx106-elf-gcc
CFLAGS += -I$(ESP_SDK)/include -mlongcalls -I. #-I$(ESP_DRIVER_LIB) -I $(ESP_DRIVER_LIB)/include
LDFLAGS += -L$(ESP_SDK)/lib -L$(ESP_SDK)/ld -T$(ESP_SDK)/ld/eagle.app.v6.ld
LDLIBS = -nostdlib -Wl,--no-check-sections -u call_user_start -Wl,-static -Wl,--start-group -lmain -lnet80211 -lwpa -llwip -lhal -lpp -lphy -lc -lgcc -Wl,--end-group -fno-inline-functions -mtext-section-literals 

# Application-specific setup
CFLAGS += -Wall

%$(IMAGE1_SUFFIX) %$(IMAGE2_SUFFIX) : %
	$(ESPTOOL) elf2image $< 

all: $(BIN)

image: $(BIN)$(IMAGE1_SUFFIX) $(BIN)$(IMAGE2_SUFFIX)

flash: image
	$(ESPTOOL) write_flash 0 $(BIN)$(IMAGE1_SUFFIX) 0x10000 $(BIN)$(IMAGE2_SUFFIX)

clean:
	$(RM) $(BIN)
	$(RM) $(BIN)$(IMAGE1_SUFFIX) $(BIN)$(IMAGE2_SUFFIX)

