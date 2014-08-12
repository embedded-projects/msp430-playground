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
 * @file RoundRobinScheduler.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief A simple sequential scheduler. Obtains a map of active sensors that
 * are scheduled.
 *
 **/

#include "Sensor/RoundRobinScheduler.h"

volatile int Sensor_RoundRobinScheduler_currentSensor = -1;

int Sensor_RoundRobinScheduler_activeSensors[8] = {1, 1, 1, 1, 1, 1, 1, 1}; /**< a map of active sensors, 1 is active, 0 is not */

/**
 * sets the active sensors by array[8] of int
 * array is copied, thus it can be cleared after function call
 */
void Sensor_RoundRobinScheduler_SetActiveSensors(int* sensors) {
	int i;

	for(i = 0; i < 8; i++) {
		Sensor_RoundRobinScheduler_activeSensors[i] = sensors[i];
	}

	return;
}

/**
 * initializes this scheduler
 */
void Sensor_RoundRobinScheduler_Init() {

}

/**
 * returns the current sensor
 */
int Sensor_RoundRobinScheduler_GetCurrentSensor() {
	return Sensor_RoundRobinScheduler_currentSensor;
}

/**
 * moves through all sensors and determines the next one that should be active
 * it internally sets the determined sensor as the current sensor
 * @return the determined next sensor
 */
int Sensor_RoundRobinScheduler_DetermineNewSensor() {
	int candidate = Sensor_RoundRobinScheduler_currentSensor;
	int j = 0;

	do {
		candidate = (++candidate > 7) ? 0 : candidate;
		if(Sensor_RoundRobinScheduler_activeSensors[candidate] > 0) {
			Sensor_RoundRobinScheduler_currentSensor = candidate;
			return candidate;
		}
	} while(++j < 10);

	Sensor_RoundRobinScheduler_currentSensor = -1;
	return -1;
}

//EOF
