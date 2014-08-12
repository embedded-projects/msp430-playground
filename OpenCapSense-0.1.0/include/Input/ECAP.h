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
 * @file ECAP.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef INPUT_ECAP_H
#define INPUT_ECAP_H

#include "DSP28x_Project.h"
#include "F2806x_Ecap.h"
#include "Utilities/SimpleBuffer.h"

// Error-Codes
#define INPUT_ECAP_SUCCESS 0
#define INPUT_ECAP_INVALID_ECAP 1
#define INPUT_ECAP_INVALID_ISR 2
#define INPUT_ECAP_INACTIVE 3
#define INPUT_ECAP_INVALID_BUFFER 4
#define INPUT_ECAP_ILLEGAL_ECAP 5
#define INPUT_ECAP_DISMISSCYCLES 5

int Input_ECAP_Init(int gpioNumber);
int Input_ECAP_Uninit(int gpioNumber);
int Input_ECAP_Enable(int gpioNumber);
int Input_ECAP_Disable(int gpioNumber);
interrupt void Input_ECAP_ECAP1_ISR(void);
interrupt void Input_ECAP_ECAP2_ISR(void);
interrupt void Input_ECAP_ECAP3_ISR(void);
int Input_ECAP_SwitchPin(int gpioNumber);
void Input_ECAP_SaveResult(int isrNumber);
int Input_ECAP_GetSensorNumberForGPIO(int gpioNumber);
int Input_ECAP_GetSensorNumberForECAP(int ecapNumber);
int Input_ECAP_GetEcapNumber(int gpioNumber);
void Input_ECAP_DisableInterrupt(int ecapNumber);
void Input_ECAP_SetDismissCycles(int number);
void Input_ECAP_EnableInterrupt(int ecapNumber);
int Input_ECAP_GetValueCountWithoutDismissCycles(int sensorNumber);
float Input_ECAP_GetResult(int sensorNumber);
void Input_ECAP_ClearResult(int sensorNumber);

#endif

//EOF
