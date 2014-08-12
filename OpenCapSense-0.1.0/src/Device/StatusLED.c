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
 * @file StatusLED.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#include "Device/StatusLED.h"

/**
 * initializes the status led on gpio 23
 */
void Device_StatusLED_Init() {
	Output_Pins_Init(23);
	Output_Pins_Set(23, 1);
}

/**
 * toggles led
 */
void Device_StatusLED_Toggle() {
	Output_Pins_Toggle(23);
}

/**
 * on led
 */
void Device_StatusLED_On() {
	Output_Pins_Set(23, 1);
}

/**
 * off led
 */
void Device_StatusLED_Off() {
	Output_Pins_Set(23, 0);
}

//EOF
