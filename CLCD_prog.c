#include "STD_TYPES.h"
#include <util/delay.h>

#include "BIT_MATH.h"
#include "ERR_TYPE.h"

#include "DIO_interface.h"

#include "CLCD_inteface.h"
#include "CLCD_cfg.h"
#include "CLCD_prv.h"

#if CLCD_u8CONNECTION_MODE == FOUR_BIT
static void CLCD_voidSetHalfDataPort(uint8 copy_u8FourBitData)
{
	DIO_u8SetPinValue(CLCD_u8DATA_PORT,CLCD_u8D4_PIN,GET_BIT(copy_u8FourBitData,0));
	DIO_u8SetPinValue(CLCD_u8DATA_PORT,CLCD_u8D5_PIN,GET_BIT(copy_u8FourBitData,1));
	DIO_u8SetPinValue(CLCD_u8DATA_PORT,CLCD_u8D6_PIN,GET_BIT(copy_u8FourBitData,2));
	DIO_u8SetPinValue(CLCD_u8DATA_PORT,CLCD_u8D7_PIN,GET_BIT(copy_u8FourBitData,3));
}
#endif

static void CLCD_voidSendEnablePulse(void)
{
	DIO_u8SetPinValue(CLCD_u8CTRL_PORT,CLCD_u8E_PIN,DIO_PIN_HIGH);
	_delay_ms(2);
	DIO_u8SetPinValue(CLCD_u8CTRL_PORT,CLCD_u8E_PIN,DIO_PIN_LOW);
}

void CLCD_voidSendCommand(uint8 copy_u8Command)
{
	/* 1- Set RS pin to low for command */
	DIO_u8SetPinValue(CLCD_u8CTRL_PORT,CLCD_u8RS_PIN,DIO_PIN_LOW);

	/* 2- Set RW pint to low for write */
#if CLCD_u8READ_OPERATION_ENABLE == ENABLED
	DIO_u8SetPinValue(CLCD_u8CTRL_PORT,CLCD_u8RW_PIN,DIO_PIN_LOW);
#endif

#if CLCD_u8CONNECTION_MODE == EIGHT_BIT
	/* 3- Send the command */
	DIO_u8SetPortValue(CLCD_u8DATA_PORT,copy_u8Command);

	/* 4- Send Enable pulse */
	CLCD_voidSendEnablePulse();
#elif CLCD_u8CONNECTION_MODE == FOUR_BIT
	/* 3- Send the higher order bits Command D4,D5,D6,D7 */
	CLCD_voidSetHalfDataPort(copy_u8Command >> 4u);

	/* 4- Send Enable pulse */
	CLCD_voidSendEnablePulse();

	/* 3- Send the Lower order bits command D0,D1,D2,D3 */
	CLCD_voidSetHalfDataPort(copy_u8Command);

	/* 4- Send Enable pulse */
	CLCD_voidSendEnablePulse();
#endif

}

void CLCD_voidSendData(uint8 copy_u8Data)
{
	/* 1- Set RS pin to high for data */
	DIO_u8SetPinValue(CLCD_u8CTRL_PORT,CLCD_u8RS_PIN,DIO_PIN_HIGH);

	/* 2- Set RW pint to low for write */
#if CLCD_u8READ_OPERATION_ENABLE == ENABLED
	DIO_u8SetPinValue(CLCD_u8CTRL_PORT,CLCD_u8RW_PIN,DIO_PIN_LOW);
#endif

#if CLCD_u8CONNECTION_MODE == EIGHT_BIT
	/* 3- Send the data */
	DIO_u8SetPortValue(CLCD_u8DATA_PORT,copy_u8Data);

	/* 4- Send Enable pulse */
	CLCD_voidSendEnablePulse();
#elif CLCD_u8CONNECTION_MODE == FOUR_BIT
	/* 3- Send the higher order bits Data D4,D5,D6,D7 */
	CLCD_voidSetHalfDataPort(copy_u8Data >> 4u);

	/* 4- Send Enable pulse */
	CLCD_voidSendEnablePulse();

	/* 3- Send the Lower order bits Data D0,D1,D2,D3 */
	CLCD_voidSetHalfDataPort(copy_u8Data);

	/* 4- Send Enable pulse */
	CLCD_voidSendEnablePulse();
#endif

}

void CLCD_voidGoToXY(uint8 copy_u8XPos,uint8 copy_u8YPos)
{
	uint8 Local_u8DDRamAddress = (copy_u8YPos * 0x40) + copy_u8XPos;

	/* set bit 7 for DDRam Address setting command */
	SET_BIT(Local_u8DDRamAddress,7u);

	/* Send the DDRam Address setting command */
	CLCD_voidSendCommand(Local_u8DDRamAddress);
}

void CLCD_voidInit(void)
{
	/* 1- Wait for more than 30ms after power on */
	_delay_ms(40);

	/* 2- send Function set command: 2 lines, font size = 5 x 7 */
#if CLCD_u8CONNECTION_MODE == EIGHT_BIT
	CLCD_voidSendCommand(0b00111000);
#elif CLCD_u8CONNECTION_MODE == FOUR_BIT
	CLCD_voidSetHalfDataPort(0b00000010);
	CLCD_voidSendEnablePulse();
	CLCD_voidSetHalfDataPort(0b00000010);
	CLCD_voidSendEnablePulse();
	CLCD_voidSetHalfDataPort(0b00001000);
	CLCD_voidSendEnablePulse();
#endif

	/* 3- Display on/off control: Display ON, Cursor off, blink cursor off */
	CLCD_voidSendCommand(0b00001100);

	/* 4- Display Clear */
	CLCD_voidSendCommand(1);


}

void CLCD_voidSendString(char * const copy_pString)
{
	uint8 Local_u8Counter = 0;

	for(Local_u8Counter = 0 ; *(copy_pString + Local_u8Counter) != '\0' ; Local_u8Counter++)
	{
		CLCD_voidSendData(*(copy_pString + Local_u8Counter));
	}

}

void CLCD_voidSendNumber(sint32 copy_s32Number)
{
	if(copy_s32Number == 0)
	{
		CLCD_voidSendData('0');
	}
	else
	{
		char Local_u8Digits[15];
		uint8 Local_s8Counter1 = 0;
		sint8 Local_s8Counter2 = 0;

		if(copy_s32Number < 0)
		{
			CLCD_voidSendData('-');
			copy_s32Number *= -1;
		}

		while(copy_s32Number > 0)
		{
			Local_u8Digits[Local_s8Counter1] = (copy_s32Number % 10) + 48;
			copy_s32Number /= 10;
			Local_s8Counter1++;
		}

		for(Local_s8Counter2 = ((sint8)Local_s8Counter1) - 1 ; Local_s8Counter2 >= 0 ; Local_s8Counter2--)
		{
			CLCD_voidSendData(Local_u8Digits[(uint8)Local_s8Counter2]);
		}
	}
}

uint8 CLCD_u8WriteSpecialCharacter(uint8 copy_u8PatternNumber, uint8 copy_pu8PatternArr[],uint8 copy_u8XPos,uint8 copy_u8YPos)
{
	uint8 Local_u8ErrorState = OK;

	if(copy_pu8PatternArr != NULL)
	{
		/* 1- Calculate CGRAM address */
		uint8 Local_u8CGRamAddress = copy_u8PatternNumber * 8u;

		/*2- Set CGRAM address */
		SET_BIT(Local_u8CGRamAddress,6);
		CLCD_voidSendCommand(Local_u8CGRamAddress);

		/*3- Write the pattern */
		sint8 Local_s8Counter;
		for(Local_s8Counter = 0 ; Local_s8Counter < 8 ; Local_s8Counter++)
		{
			CLCD_voidSendData(copy_pu8PatternArr[Local_s8Counter]);
		}

		/*4-Set DDRam address*/
		CLCD_voidGoToXY(copy_u8XPos,copy_u8YPos);

		/*5- send the pattern using its address */
		CLCD_voidSendData(copy_u8PatternNumber);
	}
	else
	{
		Local_u8ErrorState = NOK;
	}

	return Local_u8ErrorState;
}
