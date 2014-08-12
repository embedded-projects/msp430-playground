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
 * @file FixedParallelScheduler.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef SENSOR_FIXEDPARALLELSCHEDULER_H
#define SENSOR_FIXEDPARALLELSCHEDULER_H

#include "DSP28x_Project.h"
#include "Main.h"

int Sensor_FixedParallelScheduler_GetCurrentSensor(int id);
int Sensor_FixedParallelScheduler_DetermineNewSensors();
void Sensor_FixedParallelScheduler_Init();
int Sensor_FixedParallelScheduler_IsRoundCompleted();

#endif

//EOF
