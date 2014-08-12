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
 *
 **/

#include "Applications/AstableMultivibrator.h"

#define APPLICATIONS_ASTABLEMULTIVIBRATOR

#ifdef APPLICATIONS_ASTABLEMULTIVIBRATOR

int Applications_AstableMultivibrator_activeSensors[8] = {1, 1, 1, 1, 1, 1, 1, 1};
long Applications_AstableMultivibrator_roundedResults[8] = {0, 0, 0, 0, 0, 0, 0, 0};

int Applications_AstableMultivibrator_timer = 1;

// these values are set by callback routines
volatile int Applications_AstableMultivibrator_heartbeatFlag = 0;
volatile int Applications_AstableMultivibrator_updateFlag = 0;
float* Applications_AstableMultivibrator_currentBuffer = 0;
volatile float32 Applications_AstableMultivibrator_currentValue = 0.0; /**< the current sensor value */

void Applications_AstableMultivibrator_TimerSensorCallback();

void Applications_AstableMultivibrator_Run() {
	int i;

	// enable ecap on all pins
	Device_Switch_Init();
	Device_Switch_AllChannels(1);

	// disable status led
	Device_StatusLED_Init();
	Device_StatusLED_On();

	// initialize com port communication
	Device_FT230X_Init();
	Com_SCI_Init();

	Utilities_Timer_Generate(Applications_AstableMultivibrator_timer, 200, &Applications_AstableMultivibrator_TimerSensorCallback);
	Utilities_Timer_Stop(Applications_AstableMultivibrator_timer);

	// initialize heartbeat
	Utilities_HeartbeatFSM_Init(2);

	// init scheduling
	Sensor_RoundRobinScheduler_SetActiveSensors(Applications_AstableMultivibrator_activeSensors);
	Sensor_RoundRobinScheduler_Init();
	int nextSensor = Sensor_RoundRobinScheduler_DetermineNewSensor();

	// initialize all sensors
	for(i = 0; i < 8; i++) {
		if(Applications_AstableMultivibrator_activeSensors[i] > 0) {
			Sensor_AstableMultivibrator_Init(i);
		}
	}

	// enable the watchdog timer
	Device_Init_Watchdog();

	Utilities_Timer_Start(Applications_AstableMultivibrator_timer);
	Sensor_AstableMultivibrator_Start(nextSensor);

	// iterate over all sensors
	for(;;) {
		ServiceDog();

		if(Utilities_HeartbeatFSM_IsNext() > 0) {
			Utilities_HeartbeatFSM_Next();
		}

		// this means we're in master mode and we do what we want
		if(Applications_AstableMultivibrator_updateFlag > 0) {
			int currentSensor = 0;
			int newSensor = 0;
			currentSensor = Sensor_RoundRobinScheduler_GetCurrentSensor();
			newSensor = Sensor_RoundRobinScheduler_DetermineNewSensor();

			Sensor_AstableMultivibrator_Stop(currentSensor);
			Sensor_AstableMultivibrator_Start(newSensor);

			float average = Sensor_AstableMultivibrator_GetAverageResult(currentSensor);
			Applications_AstableMultivibrator_roundedResults[currentSensor] = (long) (average * 100) + 0.5; // cast to int + 0.5 is similar to round

			Com_SCI_TransmitValueMessage(0, currentSensor, Applications_AstableMultivibrator_roundedResults[currentSensor]);

			Applications_AstableMultivibrator_updateFlag = 0;
		}
   }
}

/**
 * callback for the sensor timer
 */
void Applications_AstableMultivibrator_TimerSensorCallback() {
	Applications_AstableMultivibrator_updateFlag = 1;
}

#endif

//EOF
