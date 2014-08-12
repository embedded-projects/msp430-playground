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
 * @file Receiver.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef SENSOR_RECEIVER_H
#define SENSOR_RECEIVER_H

#include "DSP28x_Project.h"
#include "Output/Pins.h"
#include "Input/ecap.h"
#include <math.h>

/**
 * Error-Codes
 */
#define SENSOR_RECEIVER_SUCCESS 0
#define SENSOR_RECEIVER_INVALID_SENSORNUMBER 1
#define SENSOR_RECEIVER_INVALID_CALLBACK 2
#define SENSOR_RECEIVER_ILLEGAL_SENSORNUMBER 3
#define SENSOR_RECEIVER_PIN_NOT_INITIALIZED 4
#define SENSOR_RECEIVER_ECAP_NOT_INITIALIZED 5

#define SENSOR_RECEIVER_BUFFERSIZE 128

int Sensor_Receiver_Init(int sensorNumber, void (*callback)(float, int), float (*filter)(float*, int));

int Sensor_Receiver_Sensor_To_ADC(int sensorNumber);

int Sensor_Receiver_Start(int sensorNumber);

int Sensor_Receiver_Stop(int sensorNumber);

#endif

//EOF
