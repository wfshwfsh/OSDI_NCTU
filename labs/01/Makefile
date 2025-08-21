#CROSS_COMPILE ?= arm-linux-gnueabihf-
CROSS_COMPILE ?= aarch64-linux-gnu-
CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS  = -Wall -O2 -ffreestanding -nostdlib -nostartfiles
LDFLAGS = -T linker_rpi.ld

OBJS = start.o bootloader.o

all: kernel.img

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

kernel.elf: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

kernel.img: kernel.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f *.o *.elf *.img
