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
 * @file SimpleBuffer.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief A buffer with a variable number of segments. The segments are filled
 * item by item. When a segment is full, the buffer will switch to the next segment.
 * This buffer is used for ADC sampling when operations are executed on one segment
 * while the other segment is filled with new data and vice-versa.
 *
 **/

#include "Utilities/SimpleBuffer.h"

Uint16 Utilities_SimpleBuffer_lock = 0;

/** 
 * inserts an item into the buffer
 * @param the item
 * @param the buffer
 * @return status of insertion
 **/
#pragma CODE_SECTION(Utilities_SimpleBuffer_In, "ramfuncs");
Uint16 Utilities_SimpleBuffer_In(float32 item, Utilities_SimpleBuffer* buffer) {
	if(buffer->write >= buffer->size) {
		return UTILITIES_SIMPLE_BUFFER_FULL;
	} else if(Utilities_SimpleBuffer_lock == 1) {
		return UTILITIES_SIMPLE_BUFFER_LOCKED;	
	}
		
	buffer->data[buffer->segment][buffer->write] = item;
  	buffer->write++;
  
  	return UTILITIES_SIMPLE_BUFFER_SUCCESS;
}

/** 
 * resets the values in the buffer
 * @param the buffer
 **/
#pragma CODE_SECTION(Utilities_SimpleBuffer_Flush, "ramfuncs");
Uint16 Utilities_SimpleBuffer_Flush(Utilities_SimpleBuffer* buffer) {
	Uint16 i;
	
	for(i = 0; i < buffer->size; i++) {
		buffer->data[buffer->segment][i] = 0.0;
	}
	
	buffer->write = 0;

  	return UTILITIES_SIMPLE_BUFFER_SUCCESS;
}

/**
 * finalizes a segment and returns the pointer to the finalized segment
 * @return last finalized segment
 **/
#pragma CODE_SECTION(Utilities_SimpleBuffer_FinalizeSegment, "ramfuncs");
float32* Utilities_SimpleBuffer_FinalizeSegment(Utilities_SimpleBuffer* buffer) {
	Uint16 finalizedSegment;
	
	Utilities_SimpleBuffer_lock = 1;
	
	finalizedSegment = buffer->segment;
	buffer->write = 0;
	
	buffer->segment++;
	
	if(buffer->segment >= buffer->segmentSize) {
		buffer->segment = 0;
	}
	
	Utilities_SimpleBuffer_lock = 0;
	
	return buffer->data[finalizedSegment];
}

/**
 * resets the buffer
 **/
#pragma CODE_SECTION(Utilities_SimpleBuffer_Reset, "ramfuncs");
Uint16 Utilities_SimpleBuffer_Reset(Utilities_SimpleBuffer* buffer) {	
	Utilities_SimpleBuffer_lock = 1;
	
	buffer->write = 0;
	buffer->segment = 0;
	buffer->segmentSize = UTILITIES_SIMPLE_BUFFER_SEGMENTS;
	
	Utilities_SimpleBuffer_lock = 0;
	
	return UTILITIES_SIMPLE_BUFFER_SUCCESS;
}

/**
 * @return the status of the buffer
 **/
#pragma CODE_SECTION(Utilities_SimpleBuffer_Status, "ramfuncs");
Uint16 Utilities_SimpleBuffer_Status(Utilities_SimpleBuffer* buffer) {
	if(buffer->write >= buffer->size) {
		return UTILITIES_SIMPLE_BUFFER_FULL;
	} else if(buffer->write > 0) { 
		return UTILITIES_SIMPLE_BUFFER_FILLED;
	} else if(buffer->write == 0) { 
		return UTILITIES_SIMPLE_BUFFER_EMPTY;
	}
	
	return 0;
}

// EOF
