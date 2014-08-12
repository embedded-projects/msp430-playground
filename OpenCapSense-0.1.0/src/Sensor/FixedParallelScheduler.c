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
 * @file AstableMultivibrator.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief This module is a scheduler that takes care of scheduling two sensors in parallel.
 * You are able to change the schedule, be aware that pins may be located on the same ecap.
 * This should be the optimal solution for parallel sampling.
 *
 **/

#include "Sensor/FixedParallelScheduler.h"

int Sensor_FixedParallelScheduler_currentSensors = -1;

int Sensor_FixedParallelScheduler_schedule[4][2] = {{0, 3}, {1, 5}, {4, 6}, {2, 7}};

/**
 * initializes this scheduler, schedules two sensors in parallel
 */
void Sensor_FixedParallelScheduler_Init() {

}

/**
 * returns the current sensor
 * @param the id within 0 and 1
 */
int Sensor_FixedParallelScheduler_GetCurrentSensor(int id) {
	if(Sensor_FixedParallelScheduler_currentSensors != -1 && id >= 0 && id <= 1) {
		return Sensor_FixedParallelScheduler_schedule[Sensor_FixedParallelScheduler_currentSensors][id];
	} else {
		return 0;
	}
}


/**
 * returns if the current round is complete
 */
int Sensor_FixedParallelScheduler_IsRoundCompleted() {
	if(Sensor_FixedParallelScheduler_currentSensors == 3) {
		return 1;
	} else {
		return 0;
	}
}

/**
 * moves through all sensors and determines the next one that should be active
 * it internally sets the determined sensor as the current sensor
 * @return the determined next sensor
 */
int Sensor_FixedParallelScheduler_DetermineNewSensors() {
	if(Sensor_FixedParallelScheduler_currentSensors == -1) {
		Sensor_FixedParallelScheduler_currentSensors = 0;
	} else {
		Sensor_FixedParallelScheduler_currentSensors = Sensor_FixedParallelScheduler_currentSensors + 1;
		if(Sensor_FixedParallelScheduler_currentSensors > 3) {
			Sensor_FixedParallelScheduler_currentSensors = 0;
		}
	}

	return Sensor_FixedParallelScheduler_currentSensors;
}

//EOF
