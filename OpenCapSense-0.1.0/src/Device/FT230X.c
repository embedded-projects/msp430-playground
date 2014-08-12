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
 * @file FT230X.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#include "Device/FT230X.h"

/**
 * initializes the adc/ecap SPDT switch
 * @return DEVICE_SWITCH_SUCCESS on success
 */
int Device_FT230X_Init() {
	// initialize RFR to FT230X
	Output_Pins_Init(12);
	Output_Pins_Set(12, 0);

	// enable CTS to FT230X
	GpioCtrlRegs.GPAPUD.bit.GPIO25 = 0;   // Enable pullup on GPIO25
	GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;  // GPIO25 = GPIO
	GpioCtrlRegs.GPADIR.bit.GPIO25 = 1;

	return DEVICE_FT230X_SUCCESS;
}

//EOF
