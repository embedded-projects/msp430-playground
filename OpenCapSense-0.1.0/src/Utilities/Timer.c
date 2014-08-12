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
 * @file Timer.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief Timer functionality using EPWMs
 *
 **/

#include "Utilities/Timer.h"

volatile struct EPWM_REGS *Utilities_Timer_epwms[4] = {&EPwm1Regs, &EPwm2Regs, &EPwm3Regs, &EPwm4Regs};//the used epwms
void (*Utilities_Timer_callback[4])(void) = {0, 0, 0, 0};
int Utilities_Timer_limit[4] = {1, 1, 1, 1};
int Utilities_Timer_counter[4] = {0, 0, 0, 0};

int Utilties_Timer_isInitialized = 0;

/**
 * initializes a general purpose timer
 * @param timernumber a number to identify the timer (between 0 and 3)
 * @param frequency the frequency in Hz at which the timer triggers (should never be higher than 100k but can go down to mHz)
 * @param isr the function which is called when the timer triggers
 * @return returns an error code or 0 if successful
 */
int Utilities_Timer_Generate(int timernumber, float frequency, void (*callback)(void)) {
	if(timernumber > 3 || timernumber < 0) {
		return UTILITIES_TIMER_INVALID;
	}
	if(callback == 0) {
		return UTILITIES_TIMER_INVALID_CALLBACK;
	}
	float period;
	period = 40000000 / frequency;
	Utilities_Timer_limit[timernumber] = 1;
	Utilities_Timer_counter[timernumber] = 0;
	IER |= M_INT3;

	while(period > 65534) {
		Utilities_Timer_limit[timernumber] = Utilities_Timer_limit[timernumber] * 2;
		period = period * 0.5;
	}

	Utilities_Timer_callback[timernumber] = callback;

	Utilities_Timer_epwms[timernumber]->TBCTL.bit.PHSEN = TB_ENABLE;
	Utilities_Timer_epwms[timernumber]->TBPRD = (Uint32) period;
	Utilities_Timer_epwms[timernumber]->TBCTL.bit.CTRMODE = TB_COUNT_UP;		// Count down
	Utilities_Timer_epwms[timernumber]->ETSEL.bit.INTSEL = ET_CTR_PRD;     	// Select INT on Zero,
	Utilities_Timer_epwms[timernumber]->ETSEL.bit.INTEN = 1;					// Enable INT
	Utilities_Timer_epwms[timernumber]->ETPS.bit.INTPRD = ET_1ST;				// Generate INT on first event for higher clock-speed

	EALLOW;
		PieVectTable.EPWM1_INT = &Utilities_Timer0_ISR;
		PieVectTable.EPWM2_INT = &Utilities_Timer1_ISR;
		PieVectTable.EPWM3_INT = &Utilities_Timer2_ISR;
		PieVectTable.EPWM4_INT = &Utilities_Timer3_ISR;
		PieCtrlRegs.PIEIER3.all |= 1 << (timernumber);
	EDIS;

	Utilties_Timer_isInitialized = 1;

	return UTILITIES_TIMER_SUCCESS;
}

/**
 * isr for timer0 = epwm1 (only used internally)
 */
interrupt void Utilities_Timer0_ISR(void) {
	Utilities_Timer_counter[0]++;
	if(Utilities_Timer_counter[0] == Utilities_Timer_limit[0]) {
		Utilities_Timer_counter[0] = 0;
		Utilities_Timer_callback[0]();
	}
	Utilities_Timer_epwms[0]->ETCLR.bit.INT = 1;
	PieCtrlRegs.PIEACK.bit.ACK3 = 1;
}

/**
 * isr for timer2 = epwm2 (only used internally)
 */
interrupt void Utilities_Timer1_ISR(void) {
	Utilities_Timer_counter[1]++;
	if(Utilities_Timer_counter[1] == Utilities_Timer_limit[1]) {
		Utilities_Timer_counter[1] = 0;
		Utilities_Timer_callback[1]();
	}
	Utilities_Timer_epwms[1]->ETCLR.bit.INT = 1;
	PieCtrlRegs.PIEACK.bit.ACK3 = 1;
}

/**
 * isr for timer2 = epwm3 (only used internally)
 */
interrupt void Utilities_Timer2_ISR(void) {
	Utilities_Timer_counter[2]++;
	if(Utilities_Timer_counter[2] == Utilities_Timer_limit[2]) {
		Utilities_Timer_counter[2] = 0;
		Utilities_Timer_callback[2]();
	}
	Utilities_Timer_epwms[2]->ETCLR.bit.INT = 1;
	PieCtrlRegs.PIEACK.bit.ACK3 = 1;
}

/**
 * isr for timer3 = epwm4 (only used internally)
 */
interrupt void Utilities_Timer3_ISR(void) {
	Utilities_Timer_counter[3]++;
	if(Utilities_Timer_counter[3] == Utilities_Timer_limit[3]) {
		Utilities_Timer_counter[3] = 0;
		Utilities_Timer_callback[3]();
	}
	Utilities_Timer_epwms[3]->ETCLR.bit.INT = 1;
	PieCtrlRegs.PIEACK.bit.ACK3 = 1;
}


/**
 * stops one of the counters
 * @param timernumber the number of the counter (between 0 and 3)
 * @return returns an error code or 0 if successful
 */
int Utilities_Timer_Stop(int timernumber) {
	if(timernumber > 3 || timernumber < 0) {
		return UTILITIES_TIMER_INVALID;
	}
	if(Utilities_Timer_callback[timernumber] == 0) {
		return UTILITIES_TIMER_UNUSED;
	}
	Utilities_Timer_epwms[timernumber]->TBCTL.bit.CTRMODE = TB_FREEZE;
	return UTILITIES_TIMER_SUCCESS;
}

/**
 * starts one of the stopped counters
 * @param timernumber the number of the counter (between 0 and 3)
 * @return returns an error code or 0 if successful
 */
int Utilities_Timer_Start(int timernumber) {
	if(timernumber > 3 || timernumber < 0) {
		return UTILITIES_TIMER_INVALID;
	}
	if(Utilities_Timer_callback[timernumber] == 0) {
		return UTILITIES_TIMER_UNUSED;
	}
	Utilities_Timer_epwms[timernumber]->TBCTR = 0;
	Utilities_Timer_epwms[timernumber]->TBCTL.bit.CTRMODE = TB_COUNT_UP;
	return UTILITIES_TIMER_SUCCESS;
}


//EOF
