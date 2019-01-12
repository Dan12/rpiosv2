extern void PUT32 ( unsigned int, unsigned int );
extern unsigned int GET32 ( unsigned int );
extern void dummy ( unsigned int );

#define GPFSEL3 0x3F20000C
#define GPFSEL4 0x3F200010
#define GPSET1  0x3F200020
#define GPCLR1  0x3F20002C

void kernel_main(void) {
  unsigned int ra;

  // set up the green led
  ra=GET32(GPFSEL4);
  ra&=~(7<<21);
  ra|=1<<21;
  PUT32(GPFSEL4,ra);

  // set up the red led
  ra=GET32(GPFSEL3);
  ra&=~(7<<15);
  ra|=1<<15;
  PUT32(GPFSEL3,ra);

  while(1)
  {
    // green on, red off
    PUT32(GPSET1,1<<(47-32));
    PUT32(GPCLR1,1<<(35-32));
    // dumb dealy function
    for(ra=0;ra<0x100000;ra++) dummy(ra);
    // green off, red on
    PUT32(GPCLR1,1<<(47-32));
    PUT32(GPSET1,1<<(35-32));
    for(ra=0;ra<0x100000;ra++) dummy(ra);
  }
}