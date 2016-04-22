
CROSS := arm-linux-gnueabihf2014.09-
SW_VER_STRING := 0.0.1

CC:=$(CROSS)gcc
LD:=$(CROSS)gcc
OBJCOPY:=$(CROSS)objcopy
SIZE:=$(CROSS)size

#######################################################################################

OBJS := plugin.o serial.o board.o

ELF := plugin.elf
BIN := plugin.imx
MAP := plugin.map

LDSCRIPT := plugin.ld

#######################################################################################

CFLAGS += -Wall --std=gnu99 -O2 -ggdb
CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += -funsigned-char -funsigned-bitfields
CFLAGS += -mlong-calls
CFLAGS += -marm
CFLAGS += -fomit-frame-pointer
CFLAGS += -MD -MP
CFLAGS += -DVERSION=$(SW_VER_STRING)

LDFLAGS += -nostartfiles -static-libgcc
LDFLAGS += -fdata-sections -ffunction-sections -mlong-calls
LDFLAGS += -Wl,--gc-sections -Wl,--build-id=none -Wl,-static

LDFLAGS += -Wl,--script=$(LDSCRIPT)
LDFLAGS += -Wl,-Map=$(MAP)

#######################################################################################

all: $(BIN)

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $^ $@
	
$(ELF): $(OBJS)
	$(CC) $(LDFLAGS) $^ -o $@
	$(SIZE) $@
	
$(OBJS): %.o: %.c
	$(CC) $(CFLAGS) $< -c -o $@

	
clean:
	-rm -f $(BIN) $(ELF) $(OBJS) $(MAP) *.d

distclean: clean
