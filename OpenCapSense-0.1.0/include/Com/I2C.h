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

#ifndef COM_I2C_H
#define COM_I2C_H

#include "DSP28x_Project.h"

/**
 * Error-Codes
 */
#define COM_I2C_SUCCESS 0
#define COM_I2C_NOT_READY 1
#define COM_I2C_BUS_BUSY 2
#define COM_I2C_NO_DATA 3
#define COM_I2C_INVALID_CALLBACK 4
#define COM_I2C_UNKNOWN_ERROR 5
#define COM_I2C_SEND_FAIL 6
#define COM_I2C_READ_FAIL 7
#define COM_I2C_INVALID_BUFFER 8
#define COM_I2C_TIMEOUT 1000000

int Com_I2C_Init(void);

int Com_I2C_WriteData_Async(int adress, int numberOfBytes, int *data, void (*callback)(int, int));

int Com_I2C_WriteData_Sync(int adress, int numberOfBytes, int *data);

int Com_I2C_ReadData_Async(int address, int numberOfBytes, int *readBuffer, void (*callback)(int, int, int, int*));

int Com_I2C_ReadData_Sync(int address, int numberOfBytes, int *readBuffer);

interrupt void Com_I2C_ISR(void);

#endif

//EOF
