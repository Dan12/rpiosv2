TOOLCHAIN = ../gcc-arm/gcc-arm-none-eabi-7-2017-q4-major/bin/arm-none-eabi
CC = $(TOOLCHAIN)-gcc
OBJCOPY = $(TOOLCHAIN)-objcopy-cpd

CPU = cortex-a7
IMG_NAME=kernel7

CFLAGS= -mcpu=$(CPU) -fpic -ffreestanding $(DIRECTIVES) -g
CSRCFLAGS= -O2 -Wall -Wextra
LFLAGS= -ffreestanding -O2 -nostdlib

KER_SRC = src

OBJ_DIR=build

# gather all .c files
KERSOURCES = $(wildcard $(KER_SRC)/*.c)
# gather all .S files
ASMSOURCES = $(wildcard $(KER_SRC)/*.S)

# make a list of all the compilation objects
OBJECTS = $(patsubst $(KER_SRC)/%.c, $(OBJ_DIR)/%.o, $(KERSOURCES))
OBJECTS += $(patsubst $(KER_SRC)/%.S, $(OBJ_DIR)/%.o, $(ASMSOURCES))

build: $(OBJECTS)
	$(CC) -T linker.ld -o $(IMG_NAME).elf $(LFLAGS) $(OBJECTS)
	$(OBJCOPY) $(IMG_NAME).elf -O binary $(IMG_NAME).img

# make all of the .c files
$(OBJ_DIR)/%.o: $(KER_SRC)/%.c
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@ $(CSRCFLAGS)

$(OBJ_DIR)/%.o: $(KER_SRC)/%.S
	mkdir -p $(@D)
	$(CC) $(CFLAGS) -c $< -o $@

run: build
	qemu-system-arm -m 256 -M raspi2 -serial stdio -kernel $(IMG_NAME).elf

clean:
	rm -rf $(OBJ_DIR)
	rm $(IMG_NAME).elf
	rm $(IMG_NAME).img