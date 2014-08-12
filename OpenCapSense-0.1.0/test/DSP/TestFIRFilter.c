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
 * @file TestFIRFilter.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief Tests the implementation of the FIR filter
 *
 **/
 
#include "MinUnit/MinUnit.h"
#include "F2806x_Device.h"
#include "DSP/FIRFilter.h"
 
char* DSP_TestFIRFilter_TestOne();

char* DSP_TestFIRFilter_AllTests() {	
	MinUnit_RunTest(DSP_TestFIRFilter_TestOne);
	
	return 0;
}

/**
 * tests the output of frequency drift compensation
 **/
char* DSP_TestFIRFilter_TestOne() {
	Uint16 i = 0;
	float32 result = 0;
	float32 test[10] = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0};
	
	DSP_FIRFilter_Init();
	
	// we assume having a kernel size of 10
	// first, generate some offset
	for(i = 0; i < 20; i++) {
		result = DSP_FIRFilter_Filter(1, DSP_FIRFILTER_KERNELTEST, -1.0);
	}
	
	result = DSP_FIRFilter_Filter(1, DSP_FIRFILTER_KERNELTEST, -1.0);
	MinUnit_Assert(abs(result + 55.0) < 0.001);
	
	// we assume having a kernel size of 10
	// first, generate some offset
	for(i = 0; i < 10; i++) {	
		result = DSP_FIRFilter_Filter(1, DSP_FIRFILTER_KERNELTEST, test[i]);
	}
	
	MinUnit_Assert(abs(result - 385.0) < 0.001);
	
	
	return 0;
}

// EOF
