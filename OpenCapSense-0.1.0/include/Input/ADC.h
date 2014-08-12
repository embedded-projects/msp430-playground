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
 * @file ADC.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef INPUT_ADC_H
#define INPUT_ADC_H

#include "DSP28x_Project.h"
#include "Utilities/SimpleBuffer.h"

// Error-Codes
#define INPUT_ADC_SUCCESS 0
#define INPUT_ADC_INVALID_PIN 1
#define INPUT_ADC_INVALID_ISR 2
#define INPUT_ADC_PIN_INACTIVE 3
#define INPUT_ADC_INVALID_FREQUENCY 4
#define INPUT_ADC_ILLEGAL_BUFFERSIZE 5
#define INPUT_ADC_INVALID_BUFFER 6

int Input_ADC_Init(int pinPair, long frequency, Utilities_SimpleBuffer *buffer1, Utilities_SimpleBuffer *buffer2, void (*isr)(float32*, float32*));
int Input_ADC_Change_Frequency(int pinPair, long frequency);
int Input_ADC_Enable(int pinPair);
int Input_ADC_Disable(int pinPair);
int Input_ADC_Change_ISR(int pinPair, void (*isr)(float32*, float32*));
interrupt void Input_ADC_A0B0_ISR(void);
interrupt void Input_ADC_A1B1_ISR(void);
interrupt void Input_ADC_A2B2_ISR(void);
interrupt void Input_ADC_A4B4_ISR(void);
void Input_ADC_FillBuffer(int pinPair, float32 result0, float32 result1);

#endif

// EOF
