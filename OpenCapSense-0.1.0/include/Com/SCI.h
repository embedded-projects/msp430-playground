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
 * @file Simulator.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef COM_SCI_H_
#define COM_SCI_H_

#include "F2806x_Device.h"
#include "F2806x_Sci.h"
#include "DSP28x_Project.h"
#include <math.h>

#define COM_SCI_BAUD_230400  230400UL
#define COM_SCI_BAUD_172800  172800UL
#define COM_SCI_BAUD_115200  115200UL
#define COM_SCI_BAUD_57600    57600UL
#define COM_SCI_BAUD_38400    38400UL
#define COM_SCI_BAUD_19200    19200UL
#define COM_SCI_BAUD_9600      9600UL
#define COM_SCI_BAUD_4800      4800UL
#define COM_SCI_BAUD_2400      2400UL
#define COM_SCI_BAUD_1200      1200UL
#define COM_SCI_BAUD_RATE   COM_SCI_BAUD_115200
#define COM_SCI_LOSPCK     20000000UL
#define COM_SCI_BRR        (COM_SCI_LOSPCK/(COM_SCI_BAUD_RATE*8) ) -1
#define COM_SCI_REQUESTBUFFERSIZE 10
#define COM_SCI_SUCCESS 0
#define COM_SCI_ERROR 1

void Com_SCI_TransmitMessage(char * msg);
void Com_SCI_Transmit(int a);
void Com_SCI_Init();
Uint16 Com_SCI_Strlen(const char * str);
void Com_SCI_CharReverse(char* s);
void Com_SCI_IntToChar(long n, char s[]);
int Com_SCI_TransmitLong(long n);
char* Com_SCI_GetRequest();
Uint16 Com_SCI_CheckRequest();
void Com_SCI_WaitForClearedBuffer();
void Com_SCI_TransmitFirmwareInformation();
void Com_SCI_TransmitValueMessage(int boardId, int sensorId, long value);

#endif /*SCI_H_*/

// EOF
