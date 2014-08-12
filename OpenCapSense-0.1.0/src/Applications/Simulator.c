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
 * @file Simulator.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#include "Applications/Simulator.h"

#ifdef APPLICATIONS_SIMULATOR

// these values are set by callback routines
volatile int Applications_Simulator_updateFlag = 0;
float* Applications_Simulator_currentBuffer = 0;
volatile float32 Applications_Simulator_currentValue = 0.0; /**< the current sensor value */

int Applications_Simulator_activeSensors[8] = {1, 1, 1, 1, 1, 1, 1, 1}; /**< a map of active sensors, 1 is active, 0 is not */

void Applications_Simulator_SensorCallback(float a, int ecapNumber);
float Applications_Simulator_FilterCallback(float* buffer, int sensorNumber);

void Applications_Simulator_Run() {
	Com_SCI_Init();

	Sensor_RoundRobinScheduler_SetActiveSensors(Applications_Simulator_activeSensors);
	Sensor_RoundRobinScheduler_Init();

	// start the timer for sensor timeout
	int nextSensor = Sensor_RoundRobinScheduler_DetermineNewSensor();

	for(;;) {
		int roundedResult = 0;
		float sensorValue = 13000 + (rand() % 100);
		roundedResult = (int) ((sensorValue) + 0.5); // cast to int + 0.5 is similar to round
		Com_SCI_TransmitValueMessage(0, nextSensor, roundedResult);

		nextSensor = Sensor_RoundRobinScheduler_DetermineNewSensor();

		DELAY_US(100000);
   }
}

#endif

//EOF
