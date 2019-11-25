# Bare Metal Raspberry Pi 2

This is my repository where I experiment with writing a bare metal operating system for my raspberry pi 2.

The code in this repo is based on various other bare metal tutorials and code bases.

The most complicated pieces to get to work were the frame buffer and timer interrupts. These difficulties most likely stemmed from the fact that most of the tutorials I looked at use a raspberry pi 1 instead of 2.

I also created a custom UART bootloader so that I don't have to constantly swap out the sd card when I want to update the kernel.

## Memory layout

0x0000

Kernel stack

0x8000

Interrupt vector

0x8010

Text section

....

Heap

## Resources

https://www.raspberrypi.org/app/uploads/2012/02/BCM2835-ARM-Peripherals.pdf

https://github.com/jsandler18/raspi-kernel

https://github.com/dwelch67/raspberrypi

## TODO

Check if you need to add volatile stuff to mailbox.

Abilities I'd like to add one day:

MMU:
https://github.com/dwelch67/raspberrypi/tree/master/mmu
https://github.com/dwelch67/raspberrypi/pull/30/files

USB Keyboard

File System
https://github.com/bztsrc/raspi3-tutorial/tree/master/0B_readsector

Utilize Mutli Core
https://github.com/dwelch67/raspberrypi/tree/master/multi00

Speed up Framebuffer
https://www.raspberrypi.org/forums/viewtopic.php?t=213964

Audio
