interrupt void KBIisr (void);
void KBIInit(void);

#define KBI_INT_ENABLE (KBI1SC |= 0x02);
#define KBI_DEBOUNCE 20 /* Debounce in tens of msec */

#define KBI_ENABLEINTERRUPTS (KBI1SC |= 0x02);
#define KBI_DISABLEINTERRUPTS (KBI1SC &= ~0x02);