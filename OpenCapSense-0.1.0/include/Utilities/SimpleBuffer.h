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
 * @file SimpleBuffer.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef UTILITIES_SIMPLEBUFFER_H
#define UTILITIES_SIMPLEBUFFER_H

#include "DSP28x_Project.h"

#define UTILITIES_SIMPLE_BUFFER_SEGMENTS 2
#define UTILITIES_SIMPLE_BUFFER_SUCCESS 0
#define UTILITIES_SIMPLE_BUFFER_FULL 1
#define UTILITIES_SIMPLE_BUFFER_FILLED 2
#define UTILITIES_SIMPLE_BUFFER_EMPTY 3
#define UTILITIES_SIMPLE_BUFFER_LOCKED 4
 
typedef struct {
  float32* data[UTILITIES_SIMPLE_BUFFER_SEGMENTS];
  Uint16 segmentSize;
  Uint16 segment;
  Uint16 write; // zeigt immer auf leeres Feld
  Uint16 size;
} Utilities_SimpleBuffer;

Uint16 Utilities_SimpleBuffer_In(float32 item, Utilities_SimpleBuffer* buffer);
float32* Utilities_SimpleBuffer_FinalizeSegment(Utilities_SimpleBuffer* buffer);
Uint16 Utilities_SimpleBuffer_Reset(Utilities_SimpleBuffer* buffer);
Uint16 Utilities_SimpleBuffer_Flush(Utilities_SimpleBuffer* buffer);
Uint16 Utilities_SimpleBuffer_Status(Utilities_SimpleBuffer* buffer);

#endif

// EOF
