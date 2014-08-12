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

#ifndef COM_CAN_H
#define COM_CAN_H

#include "DSP28x_Project.h"
#include "F2806x_ECan.h"
#include "Main.h"

#define COM_CAN_BASEADDRESS 0x9555AAA0

int Com_CAN_Init(void);
void Com_CAN_TransmitValueMessage(int sensor1, int val1, int sensor2, int val2);
void Com_CAN_ReadValueMessage(int mbox, char* messageId, int* boardId, int* sensor1, int* value1, int* sensor2, int* value2);
int Com_CAN_CheckValueMessageBoxes();
void Com_CAN_ReadStartMeasurementMessage(int* boardId);
int Com_CAN_CheckStartMeasurementMessageBox();
void Com_CAN_InitReceiveMBoxes();
void Com_CAN_InitTransmitMBoxes();
void Com_CAN_TransmitStartMeasurementMessage(int boardId);

#endif

//EOF
