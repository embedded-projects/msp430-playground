/**
 *
 * This file is part of OpenCapSense Firmware (www.opencapsense.org).
 *
 * OpenCapSense Firmware is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * OpenCapSense Firmware is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with OpenCapSense Firmware. If not, see <http://www.gnu.org/licenses/>.
 *
 * @file Pins.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief This module bundles the GPIO output functionality.
 *
 **/

#include "Output/Pins.h"

/**
 * initializes the gpio pins.
 * @param pinNumber the number of the gpio (between 0 and 39). Pin34 is configured as the LED
 * 		and must be accessed via the methods in the LED module. Make sure you don't use any gpio which is already
 * 		configured as an ADC or eCap.
 * @return an error code or 0 if successful
 */
int Output_Pins_Init(int pinNumber)
{
	Uint32 and;
	and = 0xFFFFFFFF;									//and = 1111 1111 1111 1111
	if(pinNumber > 39)
		return OUTPUT_PINS_INVALID_PIN;					//invalid Pinnumber
	if(pinNumber == 34)
		return OUTPUT_PINS_USED_FOR_LED;				//already used for LED
	if(pinNumber < 16)
	{
		and -= 1 << (2 * pinNumber);				//set 00 at pinNumber-position to configure multiplexer
		and -= 1 << (2 * pinNumber + 1);
		EALLOW;
			GpioCtrlRegs.GPAMUX1.all &= and;			//write corresponding 00 in MUX-register
			GpioCtrlRegs.GPADIR.all |= 1 << pinNumber;	//configure as output
		EDIS;
	}
	else if(pinNumber < 32)
	{
		and -= 1 << (2 * (pinNumber - 16));
		and -= 1 << (2 * (pinNumber - 16) + 1);
		EALLOW;
			GpioCtrlRegs.GPAMUX2.all &= and;
			GpioCtrlRegs.GPADIR.all |= ((Uint32) 1) << pinNumber;
		EDIS;
	}
	else if(pinNumber < 40)
	{
		and -= 1 << (2 * (pinNumber - 32));
		and -= 1 << (2 * (pinNumber - 32) + 1);
		EALLOW;
			GpioCtrlRegs.GPBMUX1.all &= and;
			GpioCtrlRegs.GPBDIR.all |= 1 << (pinNumber - 32);
		EDIS;
	}
	return OUTPUT_PINS_SUCCESS;
}

/**
 * toggles an gpio pin
 * @param pinNumber the gpio number (between 0 and 39, not 34 (LED))
 * @return an error code or 0 if successful
 */
int Output_Pins_Toggle(int pinNumber)
{
	if(pinNumber > 39)
		return OUTPUT_PINS_INVALID_PIN;					//invalid Pinnumber
	if(pinNumber == 34)
		return OUTPUT_PINS_USED_FOR_LED;				//already used for LED
	if(pinNumber < 32)
		GpioDataRegs.GPATOGGLE.all |= ((Uint32) 1) << pinNumber;	//set togglebit = 1
	else
		GpioDataRegs.GPBTOGGLE.all |= 1 << (pinNumber - 32);
	return OUTPUT_PINS_SUCCESS;
}

/**
 * sets an output pin to a value if it is not the same as notPinNumber
 * this is a convenience function for sensors
 *
 */
int Output_Pins_Set_IfNotPin(int pinNumber, int notNumber, int on) {
	if(pinNumber != notNumber) {
		return Output_Pins_Set(pinNumber, on);
	} else {
		return OUTPUT_PINS_ERROR;
	}
}

/**
 * sets the state of an gpio pin
 * @param pinNumber the gpio number (between 0 and 39, not 34 (LED))
 * @param on sets the pin to low if equals 0, high otherwise
 * @return an error code or 0 if successful
 */
int Output_Pins_Set(int pinNumber, int on)
{
	if(pinNumber > 39)
		return OUTPUT_PINS_INVALID_PIN;					//invalid Pinnumber
	if(pinNumber == 34)
		return OUTPUT_PINS_USED_FOR_LED;				//already used for LED
	if(on)
	{
		if(pinNumber < 32)
			GpioDataRegs.GPASET.all |= ((Uint32) 1) << pinNumber;	//set setbit = 1
		else
			GpioDataRegs.GPBSET.all |= 1 << (pinNumber - 32);
	}
	else
	{
		if(pinNumber < 32)
			GpioDataRegs.GPACLEAR.all |= ((Uint32) 1) << pinNumber;	//set clearbit = 1
		else
			GpioDataRegs.GPBCLEAR.all |= 1 << (pinNumber - 32);
	}
	return OUTPUT_PINS_SUCCESS;
}

//EOF
