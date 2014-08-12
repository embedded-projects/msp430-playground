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
 * @file HeartbeatFSM.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief Creates a fancy heartbeat on the status led. The state should be switched
 * (..._Next()) in the main procedure, not during interrupts to indicate a healthy
 * execution state.
 *
 **/

#include "Utilities/HeartbeatFSM.h"

int Utilities_HeartbeatFSM_state = UTILITIES_HEARTBEATFSM_STATE0_ENTRY;
int Utilities_HeartbeatFSM_heartbeatFlag = 0;
int Utilities_HeartbeatFSM_timer = 0;

void Utilities_HeartbeatFSM_TimerHeartbeatCallback();

/**
 * initializes the finite state machine
 * @param which timer to use (1-4)
 */
int Utilities_HeartbeatFSM_Init(int timer) {
	if(timer < 0 || timer > 4) {
		return -1;
	}

	Utilities_HeartbeatFSM_timer = timer;
	Utilities_HeartbeatFSM_Next();

	return 0;
}

/**
 * switches to the next state
 */
void Utilities_HeartbeatFSM_Next() {
	Utilities_HeartbeatFSM_heartbeatFlag = 0;

	Utilities_Timer_Stop(Utilities_HeartbeatFSM_timer);

	if(Utilities_HeartbeatFSM_state == UTILITIES_HEARTBEATFSM_STATE0_ENTRY) {
		Utilities_HeartbeatFSM_state = UTILITIES_HEARTBEATFSM_STATE1_OFF;
		Device_StatusLED_Off();
		Utilities_Timer_Generate(Utilities_HeartbeatFSM_timer, UTILITIES_HEARTBEATFSM_STATE1_OFF_FREQUENCY, &Utilities_HeartbeatFSM_TimerHeartbeatCallback);
	} else if(Utilities_HeartbeatFSM_state == UTILITIES_HEARTBEATFSM_STATE1_OFF) {
		Utilities_HeartbeatFSM_state = UTILITIES_HEARTBEATFSM_STATE2_ON;
		Device_StatusLED_On();
		Utilities_Timer_Generate(Utilities_HeartbeatFSM_timer, UTILITIES_HEARTBEATFSM_STATE2_ON_FREQUENCY, &Utilities_HeartbeatFSM_TimerHeartbeatCallback);
	} else if(Utilities_HeartbeatFSM_state == UTILITIES_HEARTBEATFSM_STATE2_ON) {
		Utilities_HeartbeatFSM_state = UTILITIES_HEARTBEATFSM_STATE3_SHORTOFF;
		Device_StatusLED_Off(Utilities_HeartbeatFSM_timer);
		Utilities_Timer_Generate(Utilities_HeartbeatFSM_timer, UTILITIES_HEARTBEATFSM_STATE3_SHORTOFF_FREQUENCY, &Utilities_HeartbeatFSM_TimerHeartbeatCallback);
	} else if(Utilities_HeartbeatFSM_state == UTILITIES_HEARTBEATFSM_STATE3_SHORTOFF) {
		Utilities_HeartbeatFSM_state = UTILITIES_HEARTBEATFSM_STATE4_SHORTON;
		Device_StatusLED_On();
		Utilities_Timer_Generate(Utilities_HeartbeatFSM_timer, UTILITIES_HEARTBEATFSM_STATE4_SHORTON_FREQUENCY, &Utilities_HeartbeatFSM_TimerHeartbeatCallback);
	} else if(Utilities_HeartbeatFSM_state == UTILITIES_HEARTBEATFSM_STATE4_SHORTON) {
		Utilities_HeartbeatFSM_state = UTILITIES_HEARTBEATFSM_STATE1_OFF;
		Device_StatusLED_Off();
		Utilities_Timer_Generate(Utilities_HeartbeatFSM_timer, UTILITIES_HEARTBEATFSM_STATE1_OFF_FREQUENCY, &Utilities_HeartbeatFSM_TimerHeartbeatCallback);
	} else {
		// something went wrong when the routine jumps in here
		Utilities_HeartbeatFSM_state = UTILITIES_HEARTBEATFSM_STATE1_OFF;
		Device_StatusLED_Off();
		Utilities_Timer_Generate(Utilities_HeartbeatFSM_timer, UTILITIES_HEARTBEATFSM_STATE1_OFF_FREQUENCY, &Utilities_HeartbeatFSM_TimerHeartbeatCallback);
	}

	Utilities_Timer_Start(Utilities_HeartbeatFSM_timer);
}

/**
 * determines if the current state can be switched
 * @return Indicates whether the state can be switched
 */
int Utilities_HeartbeatFSM_IsNext() {
	return Utilities_HeartbeatFSM_heartbeatFlag;
}

/**
* callback for the heartbeat timer
*/
void Utilities_HeartbeatFSM_TimerHeartbeatCallback() {
	Utilities_HeartbeatFSM_heartbeatFlag = 1;
}



//EOF
