#ifndef CLCD_PRV_H
#define CLCD_PRV_H

#define FOUR_BIT		1u
#define EIGHT_BIT		2u

#define ENABLED			3u

#define DISABLED		4u

#if CLCD_u8CONNECTION_MODE == FOUR_BIT
static void CLCD_voidSetHalfDataPort(uint8 copy_u8FourBitData);
#endif

static void CLCD_voidSendEnablePulse(void);

#endif
