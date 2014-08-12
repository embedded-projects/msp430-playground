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
 * @file TestADC.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief Tests the implementation of the timer.
 *
 **/

#include "MinUnit/MinUnit.h"
#include "F2806x_Device.h"
#include "Utilities/Timer.h"

char* Utilities_TestTimer_TestOne();
void counter1();
int count1;
void counter2();
int count2;

#define UTILITIES_TESTTIMER_ERRORMARGIN 5	//maximal relative errormargin in percent

/**
 * executes the timer tests
 */
char* Utilities_TestTimer_AllTests() {
	MinUnit_RunTest(Utilities_TestTimer_TestOne);

	return 0;
}

/**
 * two timers are generated at 10kHz and 50kHz which trigger a counter.
 * After 1ms and 2.5ms the counter value is compared against the expected value
 */
char* Utilities_TestTimer_TestOne() {
	count1 = count2 = 0;
	Utilities_Timer_Generate(0, 10000.0, counter1);
	Utilities_Timer_Generate(1, 50000.0, counter2);
	DELAY_US(1000);
	MinUnit_Assert((1 - UTILITIES_TESTTIMER_ERRORMARGIN / 100) * 50 <= count2 && count2 >= (1 + UTILITIES_TESTTIMER_ERRORMARGIN / 100) * 50);
	MinUnit_Assert((1 - UTILITIES_TESTTIMER_ERRORMARGIN / 100) * 10 <= count1 && count1 >= (1 + UTILITIES_TESTTIMER_ERRORMARGIN / 100) * 10);
	Utilities_Timer_Stop(0);

	DELAY_US(500);
	MinUnit_Assert((1 - UTILITIES_TESTTIMER_ERRORMARGIN / 100) * 10 <= count1 && count1 >= (1 + UTILITIES_TESTTIMER_ERRORMARGIN / 100) * 10);
	Utilities_Timer_Start(0);
	MinUnit_Assert((1 - UTILITIES_TESTTIMER_ERRORMARGIN / 100) * 10 <= count1 && count1 >= (1 + UTILITIES_TESTTIMER_ERRORMARGIN / 100) * 10);
	DELAY_US(1000);
	MinUnit_Assert((1 - UTILITIES_TESTTIMER_ERRORMARGIN / 100) * 20 <= count1 && count1 >= (1 + UTILITIES_TESTTIMER_ERRORMARGIN / 100) * 20);
	MinUnit_Assert((1 - UTILITIES_TESTTIMER_ERRORMARGIN / 100) * 125 <= count2 && count2 >= (1 + UTILITIES_TESTTIMER_ERRORMARGIN / 100) * 125);

	Utilities_Timer_Stop(0);
	Utilities_Timer_Stop(1);
	return 0;
}

/**
 * increments the counter for timer 0
 */
void counter1() {
	count1++;
}

/**
 * increments the counter for timer 1
 */
void counter2() {
	count2++;
}

// EOF
