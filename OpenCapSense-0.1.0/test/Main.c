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
 * @file Main.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief This module calls the MinUnit tests. In order to execute this file,
 * define TESTING in advance. By doing so the normal Main procedure will be
 * replaced by this one. There are certain tests that can only be executed with
 * additional hardware (currently UNDEFINED). MinUnit stops the debugger when
 * a test fails. At the end of all tests, MinUnit will stop the debugger
 * automatically. This indicates a successful run of all test.
 *
 **/

#include <stdio.h>
#include <stdlib.h>

#ifdef TESTING

#include "Device/Init.h"
#include "MinUnit/MinUnit.h"
#include "F2806x_Device.h"

//#define HARDWARE_TESTS

extern char* Com_TestSCI_AllTests();

extern char* Input_TestADC_AllTests();

extern char* Utilities_TestTimer_AllTests();

extern char* Sensor_TestAstableMultivibrator_AllTests();

int main(void) {
	Device_Init();
	
	Com_TestSCI_AllTests();

	Utilities_TestTimer_AllTests();

#ifdef HARDWARE_TESTS

	Input_TestADC_AllTests();

	Sensor_TestAstableMultivibrator_AllTests();

#endif

	MinUnit_Stop(); // All tests complete

	return EXIT_SUCCESS;
}

#endif

// EOF
