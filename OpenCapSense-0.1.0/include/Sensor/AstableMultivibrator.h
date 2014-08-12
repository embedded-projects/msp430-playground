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
 * @file AstableMultivibrator.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef SENSOR_ASTABLEMULTIVIBRATOR_H
#define SENSOR_ASTABLEMULTIVIBRATOR_H

#include "DSP28x_Project.h"
#include "Output/Pins.h"
#include "Input/ECAP.h"
#include "Device/Init.h"
#include "Main.h"
#include <math.h>

/**
 * Error-Codes
 */
#define SENSOR_ASTABLEMULTIVIBRATOR_SUCCESS 0
#define SENSOR_ASTABLEMULTIVIBRATOR_INVALID_SENSORNUMBER 1
#define SENSOR_ASTABLEMULTIVIBRATOR_INVALID_CALLBACK 2
#define SENSOR_ASTABLEMULTIVIBRATOR_ILLEGAL_SENSORNUMBER 3
#define SENSOR_ASTABLEMULTIVIBRATOR_PIN_NOT_INITIALIZED 4
#define SENSOR_ASTABLEMULTIVIBRATOR_ECAP_NOT_INITIALIZED 5

int Sensor_AstableMultivibrator_Init(int sensorNumber);
int Sensor_AstableMultivibrator_Start(int sensorNumber);
int Sensor_AstableMultivibrator_Stop(int sensorNumber);
float Sensor_AstableMultivibrator_GetResult(int sensorNumber);
int Sensor_AstableMultivibrator_GetValueCount(int sensorNumber);
float Sensor_AstableMultivibrator_GetAverageResult(int sensorNumber);
void Sensor_AstableMultivibrator_SetDismissCycles(int number);

#endif

//EOF
