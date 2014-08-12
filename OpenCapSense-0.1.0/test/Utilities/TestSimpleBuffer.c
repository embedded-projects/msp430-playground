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
 * @file TestSimpleBuffer.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief Tests the implementation of the simple buffer.
 *
 **/

#include "MinUnit/MinUnit.h"

#include "F2806x_Device.h"

#include "Utilities/SimpleBuffer.h"

char* Utilities_TestSimpleBuffer_TestOne();

char* Utilities_TestSimpleBuffer_AllTests() {
	MinUnit_RunTest(Utilities_TestSimpleBuffer_TestOne);
	
	//MinUnit_Stop();
	
	return 0;
}

char* Utilities_TestSimpleBuffer_TestOne() {
	Utilities_SimpleBuffer testBuffer;
	Uint16 i;
	float32 data1[128];
	float32 data2[128];
	float32* firstSegment;
	float32* secondSegment;
	
	for(i = 0; i < 128; i++) {
		data1[i] = 0;	
	}
	
	testBuffer.data[0] = &data1[0];
	testBuffer.data[1] = &data2[0];
	testBuffer.size = 128;
	testBuffer.segmentSize = 2;
	
	Utilities_SimpleBuffer_Reset(&testBuffer);

	MinUnit_Assert(Utilities_SimpleBuffer_Status(&testBuffer) == UTILITIES_SIMPLE_BUFFER_EMPTY);
	
	for(i = 0; i < 128; i++) {
		MinUnit_Assert(Utilities_SimpleBuffer_In((float) i, &testBuffer) == UTILITIES_SIMPLE_BUFFER_SUCCESS);
	}
	
	MinUnit_Assert(Utilities_SimpleBuffer_Status(&testBuffer) == UTILITIES_SIMPLE_BUFFER_FULL);
	
	MinUnit_Assert(Utilities_SimpleBuffer_In((float) 99, &testBuffer) == UTILITIES_SIMPLE_BUFFER_FULL);
	
	firstSegment = Utilities_SimpleBuffer_FinalizeSegment(&testBuffer);
	
	for(i = 0; i < 128; i++) {
		MinUnit_Assert(firstSegment[i] == (float) i);
	}
	
	for(i = 0; i < 128; i++) {
		MinUnit_Assert(Utilities_SimpleBuffer_In((float) i + 128, &testBuffer) == UTILITIES_SIMPLE_BUFFER_SUCCESS);
	}
	
	MinUnit_Assert(Utilities_SimpleBuffer_Status(&testBuffer) == UTILITIES_SIMPLE_BUFFER_FULL);
	
	MinUnit_Assert(Utilities_SimpleBuffer_In((float) 99, &testBuffer) == UTILITIES_SIMPLE_BUFFER_FULL);
	
	secondSegment = Utilities_SimpleBuffer_FinalizeSegment(&testBuffer);
	
	// check first segment, still needs to be alive
	for(i = 0; i < 128; i++) {
		MinUnit_Assert(firstSegment[i] == (float) i);
	}
	
	// check second segment
	for(i = 0; i < 128; i++) {
		MinUnit_Assert(secondSegment[i] == (float) i + 128);
	}

	// again, fill the first segment
	for(i = 0; i < 128; i++) {
		MinUnit_Assert(Utilities_SimpleBuffer_In((float) i + 256, &testBuffer) == UTILITIES_SIMPLE_BUFFER_SUCCESS);
	}

	// 
	for(i = 0; i < 128; i++) {
		MinUnit_Assert(firstSegment[i] == (float) i + 256);
	}
	
	
	return 0;
}

// EOF
