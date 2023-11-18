#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "ERR_TYPE.h"
#include <util/delay.h>

#include "DIO_interface.h"
#include "PORT_interface.h"
#include "CLCD_inteface.h"
#include "KPD_interface.h"

void main(void)
{
	PORT_voidInit();

	CLCD_voidInit();

	uint8 Local_u8Pressed_Key =	0xff;
	char Local_u8TotalArr[16] = {'#','#','#','#','#','#','#','#','#','#','#','#','#','#','#','#'};
	uint8 Local_u8TotalCTR = 0;
	uint8 Local_u8Num1Arr[7] = {0};
	uint8 Local_u8Num2Arr[7] = {0};
	sint32 Local_s32Num1 = 0;
	sint32 Local_s32Num2 = 0;
	sint8 Local_s8Counter1 = 0;
	sint8 Local_s8Counter2 = 0;
	uint32 multiplier = 1;
	uint32 multiplier1 = 1;
	uint32 multiplier2 = 1;
	sint32 Local_s32Result = 66;
	uint16 Local_u16Floating = 0;
	/* for -ve num1 */
	uint8 Local_u8flag1 = 0;
	/* for -ve num2 */
	uint8 Local_u8flag2 = 0;
	/* for division operation */
	uint8 Local_u8flag3 = 0;
	char Local_u8Op;
	uint8 Local_u8OpPosition;

	while(1)
	{
		/* Initialization after pressing clear button */
		CLCD_voidSendCommand(1);
		Local_u8Pressed_Key = 0xff;
		for(uint8 Local_u8init = 0; Local_u8init < 16;Local_u8init++)
		{
			Local_u8TotalArr[Local_u8init] = '#';
		}

		Local_u8TotalCTR = 0;

		for(uint8 Local_u8init = 0; Local_u8init < 7;Local_u8init++)
		{
			Local_u8Num1Arr[Local_u8init] = 0;
		}
		for(uint8 Local_u8init = 0; Local_u8init < 7;Local_u8init++)
		{
			Local_u8Num2Arr[Local_u8init] = 0;
		}
		Local_s32Num1 = 0;
		Local_s32Num2 = 0;
		Local_s8Counter1 = 0;
		Local_s8Counter2 = 0;
		multiplier = 1;
		multiplier1 = 1;
		multiplier2 = 1;
		Local_s32Result = 0;
		Local_u16Floating = 0;
		Local_u8flag1 = 0;
		Local_u8flag2 = 0;
		Local_u8flag3 = 0;
		_delay_ms(125);

		/* The main program */
		/* while the clear button is not pressed */
		while(Local_u8Pressed_Key != 'c')
		{
			while(Local_u8Pressed_Key != '=')
			{
				do
				{
					Local_u8Pressed_Key = KPD_u8GetPressedKey();
				}
				while(Local_u8Pressed_Key == 0xff);

				/*if the clear button is pressed at any time, break the loop of data entry */
				if(Local_u8Pressed_Key == 'c')
				{
					break;
				}

				Local_u8TotalArr[Local_u8TotalCTR] = Local_u8Pressed_Key;
				Local_u8TotalCTR++;

				/*if the clear button is pressed at any time, break the loop of data entry */

				/* if the pressed key is a number, use fn send number */
				if((Local_u8Pressed_Key >= 0) && (Local_u8Pressed_Key <= 9))
				{
					CLCD_voidSendNumber(Local_u8Pressed_Key);
					_delay_ms(125);
				}
				/* if the pressed key is an operator, use fn send data */
				else
				{
					CLCD_voidSendData(Local_u8Pressed_Key);
					_delay_ms(125);
				}
			}

			/*if the clear button was pressed, break the loop of the program in initialize every thing */
			if(Local_u8Pressed_Key == 'c')
			{
				break;
			}

			/* Analyze the entered data */
			/* Set the total data counter to zero */
			Local_u8TotalCTR = 0;
			/* Check if num1 is negative */
			if(Local_u8TotalArr[Local_u8TotalCTR] == '-')
			{
				Local_u8flag1 = 1;
				Local_u8TotalCTR++;
			}
			/* Get All digit of NUM1 till you find a character*/
			for(; (Local_u8TotalArr[Local_u8TotalCTR] >= 0) && (Local_u8TotalArr[Local_u8TotalCTR] <= 9); Local_u8TotalCTR++)
			{
				/* To not to exceed the limit of digits of num1, check if the counter is less than 7 */
				if(Local_s8Counter1 < 7)
				{
					Local_u8Num1Arr[(uint8)Local_s8Counter1] = Local_u8TotalArr[Local_u8TotalCTR];
					Local_s8Counter1++;
				}
			}
			Local_s8Counter1--;

			/* store the operator */
			Local_u8Op = Local_u8TotalArr[Local_u8TotalCTR];
			/* store where did you found the operator */
			Local_u8OpPosition = Local_u8TotalCTR;


			/* Calculate num1 */
			multiplier = 1;
			for(; Local_s8Counter1 >= 0 ; Local_s8Counter1--)
			{
				Local_s32Num1 = Local_s32Num1 + (Local_u8Num1Arr[(uint8)Local_s8Counter1] * multiplier);
				multiplier = multiplier * 10;
			}

			/* if flag1 == 1 then num1 is negative */
			if(Local_u8flag1 == 1)
			{
				Local_s32Num1 = -1 * Local_s32Num1;
			}
			/* Skip the next operators till you find a digit */
			for(; !((Local_u8TotalArr[Local_u8TotalCTR] >= 0) && (Local_u8TotalArr[Local_u8TotalCTR] <= 9)); Local_u8TotalCTR++);

			/* Check if num2 is negative and this -ve sign is not the operator itself*/
			if((Local_u8TotalArr[Local_u8TotalCTR-1] == '-') && ((Local_u8TotalCTR-1) != Local_u8OpPosition ))
			{
				Local_u8flag2 = 1;
			}
			/* Get All digit of NUM2 till you find a character*/
			for(; (Local_u8TotalArr[Local_u8TotalCTR] >= 0) && (Local_u8TotalArr[Local_u8TotalCTR] <= 9); Local_u8TotalCTR++)
			{
				/* To not to exceed the limit of digits of num2, check if the counter is less than 7 */
				if(Local_s8Counter2 < 7)
				{
					Local_u8Num2Arr[(uint8)Local_s8Counter2] = Local_u8TotalArr[Local_u8TotalCTR];
					Local_s8Counter2++;
				}
			}
			Local_s8Counter2--;

			/* Calculate num2 */
			multiplier = 1;
			for(; Local_s8Counter2 >= 0 ; Local_s8Counter2--)
			{
				Local_s32Num2 = Local_s32Num2 + (Local_u8Num2Arr[(uint8)Local_s8Counter2] * multiplier);
				multiplier = multiplier * 10;
			}

			/* if flag2 == 1 then num2 is negative */
			if(Local_u8flag2 == 1)
			{
				Local_s32Num2 = -1 * Local_s32Num2;
			}

			switch(Local_u8Op)
			{
			case '+': Local_s32Result = Local_s32Num1 + Local_s32Num2; break;
			case '-': Local_s32Result = Local_s32Num1 - Local_s32Num2; break;
			case '*': Local_s32Result = Local_s32Num1 * Local_s32Num2; break;
			case '/': Local_u8flag3 = 1; break;
			default:  Local_s32Result = Local_s32Num1 + Local_s32Num2; break;
			}

			CLCD_voidGoToXY(4u,1u);

			if((Local_u8flag3 == 1) && (Local_s32Num2 == 0))
			{
				CLCD_voidGoToXY(4u,1u);
				CLCD_voidSendString("Math Error");
			}
			else if(Local_u8flag3 == 1)
			{
				Local_s32Result = Local_s32Num1 / Local_s32Num2;
				/* Send the num */
				if(Local_s32Result == 0)
				{
					/* for a floating value less than one, we wanna show if it is -ve or +ve */
					if(((Local_s32Num1<0) && (Local_s32Num2>0)) || ((Local_s32Num1>0) && (Local_s32Num2<0)))
					{
						CLCD_voidSendData('-');
					}
				}
				CLCD_voidSendNumber(Local_s32Result);
				/* If division and there is a remainder, Calc float up to 3 digits*/
				if((Local_u8Op == '/') && (Local_s32Num1 % Local_s32Num2 != 0))
				{
					if(Local_s32Num1 < 0)
					{
						Local_s32Num1 = -1 * Local_s32Num1;
					}
					if(Local_s32Num2 < 0)
					{
						Local_s32Num2 = -1 * Local_s32Num2;
					}
					CLCD_voidSendData('.');
					multiplier1 = 10;
					multiplier2 = 10000;
					/* to avoid left zeros neglecting */
					while(((((Local_s32Num1 % Local_s32Num2) * multiplier1) / Local_s32Num2) == 0) && (multiplier1<10000))
					{
						CLCD_voidSendNumber(0);
						multiplier1 = multiplier1 * 10;
					}
					/* Left zeros will be neglected but don't worry, we already sent them above */
					Local_u16Floating = ((Local_s32Num1 % Local_s32Num2) * multiplier2) / Local_s32Num2;
					CLCD_voidSendNumber(Local_u16Floating);
				}
			}
			else
			{
				CLCD_voidSendNumber(Local_s32Result);
			}

			Local_u8Pressed_Key = 0xff;
			do
			{
				Local_u8Pressed_Key = KPD_u8GetPressedKey();
			}while(Local_u8Pressed_Key != 'c');

		}
	}




}


