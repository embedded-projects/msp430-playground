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
 * @file Timer.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef UTILITIES_TIMER_H
#define UTILITIES_TIMER_H

#include "DSP28x_Project.h"

#define UTILITIES_TIMER_SUCCESS 0
#define UTILITIES_TIMER_INVALID 1
#define UTILITIES_TIMER_INVALID_CALLBACK 2
#define UTILITIES_TIMER_UNUSED 3

int Utilities_Timer_Generate(int timernumber, float frequency, void (*isr)(void));
interrupt void Utilities_Timer0_ISR(void);
interrupt void Utilities_Timer1_ISR(void);
interrupt void Utilities_Timer2_ISR(void);
interrupt void Utilities_Timer3_ISR(void);
int Utilities_Timer_Stop(int timernumber);
int Utilities_Timer_Start(int timernumber);

#endif /* TIMER_H_ */

// EOF
