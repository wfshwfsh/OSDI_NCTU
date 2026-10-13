#CROSS_COMPILE ?= arm-linux-gnueabihf-
CROSS_COMPILE ?= aarch64-linux-gnu-
CC      = $(CROSS_COMPILE)gcc
LD      = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy

CFLAGS  = -g3 -Og -fno-omit-frame-pointer -fno-inline -fno-pie -Wall -Wextra -O0 -ffreestanding -nostdlib -nostartfiles -fno-stack-protector -fno-omit-frame-pointer

SDIR = ./
C_SRCS = $(wildcard $(SDIR)/*.c)
S_SRCS = $(wildcard $(SDIR)/*.S)
S_OBJS = $(S_SRCS:$(SDIR)/%.S=%.asmo)
C_OBJS = $(C_SRCS:$(SDIR)/%.c=%.o)

all: kernel.img

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.asmo: %.S
	$(CC) $(CFLAGS) -c $< -o $@

kernel.elf: linker.ld $(S_OBJS) $(C_OBJS)
	$(LD) -T linker.ld -o $@ $(S_OBJS) $(C_OBJS)

kernel.img: kernel.elf
	$(OBJCOPY) -O binary $< $@

clean:
	rm -f *.o *.elf *.asmo *.img
