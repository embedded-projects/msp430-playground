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
 * @file Main.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef MAIN_H_
#define MAIN_H_

#include "F2806x_Device.h"
#include "F2806x_SysCtrl.h"
#include "F2806x_PieCtrl.h"
#include "F2806x_PieVect.h"
#include "DSP28x_Project.h"

#include "Applications/Simulator.h"
#include "Applications/AstableMultivibrator.h"
#include "Output/Pins.h"
#include "Output/PWM.h"
#include "Input/ADC.h"
#include "Utilities/SimpleBuffer.h"
#include "Utilities/Timer.h"
#include "DSP/FFT.h"
#include "DSP/FrequencyDrift.h"
#include "Sensor/AstableMultivibrator.h"
#include "Device/Init.h"
#include "Device/FT230X.h"
#include "Device/StatusLED.h"
#include "Device/Switch.h"
#include "Com/SCI.h"
#include "Com/CAN.h"
#include "Sensor/Temperature.h"
#include "Sensor/RoundRobinScheduler.h"
#include "Sensor/FixedParallelScheduler.h"

#include <math.h>

void Main_Run();

#endif /*MAIN_H_*/

// EOF
