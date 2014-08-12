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
 * @file Init.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef DEVICE_INIT_H_
#define DEVICE_INIT_H_

#define DEVICE_INIT_REVISION 2 /**< the revision number of the board - if the board has an external clock, the revision is geq 3 */

#include "F2806x_Device.h"
#include "F2806x_EPwm_defines.h"  // useful defines for initialization
#include "DSP28x_Project.h"  // Device Headerfile and Examples Include File
#include "F2806x_GlobalPrototypes.h"
#include "Main.h"

void Device_Init();
void Device_Init_Watchdog();

#endif /*DEVICE_INIT_H_*/

// EOF
