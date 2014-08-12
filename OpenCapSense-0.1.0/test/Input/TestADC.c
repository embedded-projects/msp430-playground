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
 * @file TestADC.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief Tests the implementation of the ADC functionality. Requires a GPIO connected
 * to a certain ADC. This self-test is only possible with additional testing hardware.
 *
 **/

#include "MinUnit/MinUnit.h"
#include "DSP28x_Project.h"
#include "Output/Pins.h"
#include "Input/ADC.h"
#include "Utilities/SimpleBuffer.h"

char* Input_TestADC_TestOne();

void ADCCallbackTest1(float *buffer1, float *buffer2);
int adcCalled1 = 0;

#define INPUT_TESTADC_ERRORMARGIN 5	//maximal relative errormargin in percent
#define INPUT_TESTADC_TOLERANCE 2		//maximal absolute error margin

/**
 * Executes an ADC test on ADC-A0 and ADC-B0
 */
char* Input_TestADC_AllTests() {

	MinUnit_RunTest(Input_TestADC_TestOne);

	return 0;
}

/**
 * tests ADC-A0 and ADC-B0 at 64kHz
 * a rising edge is generated on gpio 00 and falling edge on gpio 01
 */
char* Input_TestADC_TestOne() {
	float adcSamples[4][64];
	Utilities_SimpleBuffer adcBuffers[2];

	adcBuffers[0].size = 64;
	adcBuffers[0].data[0] = adcSamples[0];
	adcBuffers[0].data[1] = adcSamples[1];
	adcBuffers[1].size = 64;
	adcBuffers[1].data[0] = adcSamples[2];
	adcBuffers[1].data[1] = adcSamples[3];
	Utilities_SimpleBuffer_Reset(&adcBuffers[0]);
	Utilities_SimpleBuffer_Reset(&adcBuffers[1]);

	MinUnit_Assert(Output_Pins_Init(0) == 0);
	MinUnit_Assert(Output_Pins_Init(1) == 0);

	Output_Pins_Set(0, 0);
	Output_Pins_Set(1, 1);
	Input_ADC_Init(0, 64000, &adcBuffers[0], &adcBuffers[1], &ADCCallbackTest1);
	DELAY_US(500);
	Output_Pins_Set(0, 1);
	Output_Pins_Set(1, 0);
	DELAY_US(600);
	MinUnit_Assert(adcCalled1);	//fails if callback was not called

	Input_ADC_Disable(0);

	return 0;
}

/**
 * calculates the absolute value of an float
 * @param num the float number
 * @param its absolute value
 */
float absADC(float num) {
	if(num >= 0) {
		return num;
	}
	return -num;
}

/**
 * tests the values received from both buffers
 * @param buffer1 the data array from ADC-A0
 * @param buffer2 the data array from ADC-B0
 */
void ADCCallbackTest1(float *buffer1, float *buffer2) {
	int i = 0;
	int jump1 = 0;
	int jump2 = 0;
	adcCalled1 = 1;
	for(i = 1; i < 64; i++)
	{
		if(buffer1[i] < buffer1[i - 1] * (1 - (float) INPUT_TESTADC_ERRORMARGIN / 100) || buffer1[i] > buffer1[i - 1] * (1 + (float) INPUT_TESTADC_ERRORMARGIN / 100)) {
			if(absADC(buffer1[i] - buffer1[i - 1]) > INPUT_TESTADC_TOLERANCE) {
				MinUnit_Assert(jump1 == 0);//fails if the value differs more than error margin from the previous value
				jump1++;
			}
		}
		if(buffer2[i] < buffer2[i - 1] * (1 - (float) INPUT_TESTADC_ERRORMARGIN / 100) || buffer2[i] > buffer2[i - 1] * (1 + (float) INPUT_TESTADC_ERRORMARGIN / 100)) {
			if(absADC(buffer2[i] - buffer2[i - 1]) > INPUT_TESTADC_TOLERANCE) {
				MinUnit_Assert(jump2 == 0);//fails if the value differs more than error margin from the previous value
				jump2++;
			}
		}
	}
	MinUnit_Assert(jump1 == 1);//fails if no edge was detected
	MinUnit_Assert(jump2 == 1);//fails if no edge was detected
}

// EOF
