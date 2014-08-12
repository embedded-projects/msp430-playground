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
 * @file Pins.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef OUTPUT_PINS_H
#define OUTPUT_PINS_H

#include "DSP28x_Project.h"

// Error-Codes
#define OUTPUT_PINS_SUCCESS 0
#define OUTPUT_PINS_INVALID_PIN 1
#define OUTPUT_PINS_USED_FOR_LED 2
#define OUTPUT_PINS_ERROR 3

int Output_Pins_Init(int pinNumber);
int Output_Pins_Toggle(int pinNumber);
int Output_Pins_Set(int pinNumber, int on);
int Output_Pins_Set_IfNotPin(int pinNumber, int notNumber, int on);

#endif

// EOF
