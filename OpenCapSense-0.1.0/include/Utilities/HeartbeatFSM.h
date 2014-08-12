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
 * @file HeartBeatFSM.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef UTILITIES_HEARTBEATFSM_H
#define UTILITIES_HEARTBEATFSM_H

#include "DSP28x_Project.h"
#include "Utilities/Timer.h"
#include "Device/StatusLED.h"

#define UTILITIES_HEARTBEATFSM_STATE0_ENTRY 0
#define UTILITIES_HEARTBEATFSM_STATE1_OFF 1
#define UTILITIES_HEARTBEATFSM_STATE2_ON 2
#define UTILITIES_HEARTBEATFSM_STATE3_SHORTOFF 3
#define UTILITIES_HEARTBEATFSM_STATE4_SHORTON 4

#define UTILITIES_HEARTBEATFSM_STATE1_OFF_FREQUENCY 1
#define UTILITIES_HEARTBEATFSM_STATE2_ON_FREQUENCY 8
#define UTILITIES_HEARTBEATFSM_STATE3_SHORTOFF_FREQUENCY 12
#define UTILITIES_HEARTBEATFSM_STATE4_SHORTON_FREQUENCY 16

int Utilities_HeartbeatFSM_Init(int timer);
void Utilities_HeartbeatFSM_Next();
int Utilities_HeartbeatFSM_IsNext();

#endif /* UTILITIES_HEARTBEATFSM_H */

// EOF
