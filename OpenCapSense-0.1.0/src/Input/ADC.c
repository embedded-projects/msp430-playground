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
 * @file ADC.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief This module bundles the ADC functionality. Note that switches must be
 * configured to switch from ECAP pins to ADC pins (Device_Switch_*)
 *
 **/

#include "Input/ADC.h"

void (*Input_ADC_callbackFinish[5])(float32*, float32*) = {0, 0, 0, 0, 0};

int Input_ADC_isInitialized = 0;
Utilities_SimpleBuffer *Input_ADC_buffers[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
volatile struct EPWM_REGS *Input_ADC_epwms[5] = {&EPwm7Regs, &EPwm7Regs, &EPwm7Regs, 0, &EPwm8Regs};

/**
 * initializes an ADC for further usage
 * @param pinPair the pin of the ADC(s). 0, 1, 2 or 4  are valid numbers
 * 		ADC0 = Pin19 (A) / Pin22 (B)
 * 		ADC1 = Pin18 (A) / Pin23 (B)
 * 		ADC2 = Pin17 (A) / Pin24 (B)
 * 		ADC4 = Pin16 (A) / Pin25 (B)
 * @param frequency the frequency (in Hz) at which the ADC operates (between 1,000 and 100,000)
 * @param buffer1 The initialized Utilities_SimpleBuffer struct the data from ADCxA is written into
 * @param buffer2 The Initialized Utilities_SimpleBuffer struct the data from ADCxB is written into (can be zero).
 *		If no buffer2 is specified, ADCxB is not used.
 */
int Input_ADC_Init(int pinPair, long frequency, Utilities_SimpleBuffer *buffer1, Utilities_SimpleBuffer *buffer2, void (*isr)(float32*, float32*)) {
	if(pinPair < 0 || pinPair > 4 || pinPair == 3) {
		return INPUT_ADC_INVALID_PIN;
	}
	if(frequency > 100000 || frequency < 1000) {
		return INPUT_ADC_INVALID_FREQUENCY;
	}
	if(*isr == 0) {
		return INPUT_ADC_INVALID_ISR;
	}
	if(!buffer1) {
		return INPUT_ADC_INVALID_BUFFER;
	}
	if(buffer2) {
		if(buffer1->size != buffer2->size) {
			return INPUT_ADC_ILLEGAL_BUFFERSIZE;
		}
	}

	if(Input_ADC_isInitialized == 0) {
		InitAdc();
	}

	Input_ADC_buffers[pinPair * 2] = buffer1;
	Input_ADC_buffers[pinPair * 2 + 1] = buffer2;

	EALLOW;
	  AdcRegs.ADCCTL1.bit.INTPULSEPOS = 1;              //throw interrupt after conversion
	EDIS;
	Input_ADC_callbackFinish[pinPair] = isr;

	Input_ADC_epwms[pinPair]->TBCTL.bit.PHSEN = TB_ENABLE;
	Input_ADC_epwms[pinPair]->TBPRD = 40000000 / frequency;				//period = 40M / frequency
	Input_ADC_epwms[pinPair]->TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;		//Count up and down
	Input_ADC_epwms[pinPair]->ETSEL.bit.SOCASEL = ET_CTR_PRDZERO;    	//Select INT on Zero and period
	Input_ADC_epwms[pinPair]->ETSEL.bit.SOCAEN = 1;						//Enable SOCA
	Input_ADC_epwms[pinPair]->ETPS.bit.SOCAPRD = ET_1ST;				//Generate INT on 1st event

	if(pinPair == 0) {
			IER |= M_INT1;
			EALLOW;
				AdcRegs.ADCSOC0CTL.bit.TRIGSEL = 0x11;			//EPwm7 -> SOCA0/1
				AdcRegs.ADCSOC0CTL.bit.CHSEL = 0;				//sample A0 and B0
				AdcRegs.ADCSOC0CTL.bit.ACQPS = 6;				//minimum window-size
				AdcRegs.ADCSAMPLEMODE.bit.SIMULEN0 = 1;			//enable simultanious sampling
				AdcRegs.INTSEL1N2.bit.INT1SEL = 1;				//EOC1 triggers ADCINT1
				AdcRegs.INTSEL1N2.bit.INT1E = 1;				//enable interrupt
				AdcRegs.INTSEL1N2.bit.INT1CONT = 0;				//interrupt only if flag-bit is cleared

				PieCtrlRegs.PIEIER1.bit.INTx1 = 1;				//enable ADC-interrupts
				PieVectTable.ADCINT1 = &Input_ADC_A0B0_ISR;
			EDIS;
	}
	else if(pinPair == 1) {
			IER |= M_INT1;
			EALLOW;
				AdcRegs.ADCSOC2CTL.bit.TRIGSEL = 0x11;			//EPwm7 -> SOCA2/3
				AdcRegs.ADCSOC2CTL.bit.CHSEL = 1;				//sample A1 and B1
				AdcRegs.ADCSOC2CTL.bit.ACQPS = 6;				//minimum window-size
				AdcRegs.ADCSAMPLEMODE.bit.SIMULEN2 = 1;			//enable simultaneous sampling
				AdcRegs.INTSEL1N2.bit.INT2SEL = 3;				//EOC3 triggers ADCINT2
				AdcRegs.INTSEL1N2.bit.INT2E = 1;				//enable interrupt
				AdcRegs.INTSEL1N2.bit.INT2CONT = 0;				//interrupt only if flag-bit is cleared

				PieCtrlRegs.PIEIER1.bit.INTx2 = 1;				//enable ADC-interrupts
				PieVectTable.ADCINT2 = &Input_ADC_A1B1_ISR;
			EDIS;
	}
	else if(pinPair == 2) {
			IER |= M_INT10;
			EALLOW;
				AdcRegs.ADCSOC4CTL.bit.TRIGSEL = 0x11;			//EPwm7 -> SOCA4/5
				AdcRegs.ADCSOC4CTL.bit.CHSEL = 2;				//sample A2 and B2
				AdcRegs.ADCSOC4CTL.bit.ACQPS = 6;				//minimum window-size
				AdcRegs.ADCSAMPLEMODE.bit.SIMULEN4 = 1;			//enable simultaneous sampling
				AdcRegs.INTSEL3N4.bit.INT3SEL = 5;		//---1		//EOC5 triggers ADCINT3
				AdcRegs.INTSEL3N4.bit.INT3E = 1;				//enable interrupt
				AdcRegs.INTSEL3N4.bit.INT3CONT = 0;				//interrupt only if flag-bit is cleared

				PieCtrlRegs.PIEIER10.bit.INTx3 = 1;				//enable ADC-interrupt3 in group 10
				PieVectTable.ADCINT3 = &Input_ADC_A2B2_ISR;
			EDIS;
	}
	else if(pinPair == 4) {
			IER |= M_INT10;
			EALLOW;
				AdcRegs.ADCSOC6CTL.bit.TRIGSEL = 0x13;			//EPwm8 -> SOCA6/7
				AdcRegs.ADCSOC6CTL.bit.CHSEL = 4;				//sample A4 and B4
				AdcRegs.ADCSOC6CTL.bit.ACQPS = 6;				//minimum window-size
				AdcRegs.ADCSAMPLEMODE.bit.SIMULEN6 = 1;			//enable simultanious sampling
				AdcRegs.INTSEL3N4.bit.INT4SEL = 7;				//EOC7 triggers ADCINT4
				AdcRegs.INTSEL3N4.bit.INT4E = 1;				//enable interrupt
				AdcRegs.INTSEL3N4.bit.INT4CONT = 0;				//interrupt only if flag-bit is cleared

				PieCtrlRegs.PIEIER10.bit.INTx4 = 1;				//enable ADC-interrupts
				PieVectTable.ADCINT4 = &Input_ADC_A4B4_ISR;
			EDIS;
	}

	Input_ADC_isInitialized = 1;

	return INPUT_ADC_SUCCESS;
}

/**
 * changes the frequency of an ADC
 * @param pinPair the number of the ADC (0,1,2,4)
 * @param the new frequency (in Hz, between 1,000 and 100,000)
 * @return returns an error code or 0 if successful
 */
int Input_ADC_Change_Frequency(int pinPair, long frequency) {
	if(pinPair < 0 || pinPair > 4 || pinPair == 3) {
		return INPUT_ADC_INVALID_PIN;
	}
	if(frequency > 150000 || frequency < 1000) {
		return INPUT_ADC_INVALID_FREQUENCY;
	}
	if(Input_ADC_callbackFinish[pinPair] == 0) {
		return INPUT_ADC_PIN_INACTIVE;
	}


	Input_ADC_epwms[pinPair]->TBCTR = 0;						//reset counter
	Input_ADC_epwms[pinPair]->TBPRD = 40000000 / frequency;

	return INPUT_ADC_SUCCESS;
}

/**
 * enables an specified ADC
 * @param pinPair the number of the ADC (0,1,2,4)
 * @return returns an error code or 0 if successful
 */
int Input_ADC_Enable(int pinPair) {
	if(pinPair < 0 || pinPair > 4 || pinPair == 3) {
		return INPUT_ADC_INVALID_PIN;
	}
	if(Input_ADC_callbackFinish[pinPair] == 0) {
		return INPUT_ADC_PIN_INACTIVE;
	}

	if(pinPair == 0) {
		EALLOW;
			AdcRegs.INTSEL1N2.bit.INT1E = 1;				//enable interrupt
		EDIS;
	}
	else if(pinPair == 1) {
		EALLOW;
			AdcRegs.INTSEL1N2.bit.INT2E = 1;				//enable interrupt
		EDIS;
	}
	else if(pinPair == 2) {
		EALLOW;
			AdcRegs.INTSEL3N4.bit.INT3E = 1;				//enable interrupt
		EDIS;
	}
	else if(pinPair == 4) {
		EALLOW;
			AdcRegs.INTSEL3N4.bit.INT4E = 1;				//enable interrupt
		EDIS;
	}

	return INPUT_ADC_SUCCESS;
}

/**
 * disables an specified ADC
 * @param pinPair the number of the ADC (0,1,2,4)
 * @return returns an error code or 0 if successful
 */
int Input_ADC_Disable(int pinPair) {
	if(pinPair < 0 || pinPair > 4 || pinPair == 3) {
		return INPUT_ADC_INVALID_PIN;
	}
	if(Input_ADC_callbackFinish[pinPair] == 0) {
		return INPUT_ADC_PIN_INACTIVE;
	}

	if(pinPair == 0) {
		EALLOW;
			AdcRegs.INTSEL1N2.bit.INT1E = 0;				//enable interrupt
		EDIS;
	}
	else if(pinPair == 1) {
		EALLOW;
			AdcRegs.INTSEL1N2.bit.INT2E = 0;				//enable interrupt
		EDIS;
	}
	else if(pinPair == 2) {
		EALLOW;
			AdcRegs.INTSEL3N4.bit.INT3E = 0;				//enable interrupt
		EDIS;
	}
	else if(pinPair == 4) {
		EALLOW;
			AdcRegs.INTSEL3N4.bit.INT4E = 0;				//enable interrupt
		EDIS;
	}

	return INPUT_ADC_SUCCESS;
}

/**
 * changes the interrupt service routine of an ADC which is called when the buffer(s) is/are full
 * @param pinPair the number of the ADC (0,1,2,4)
 * @param isr the new interrupt service routine
 * @return returns an error code or 0 if successful
 */
int Input_ADC_Change_ISR(int pinPair, void (*isr)(float32*, float32*)) {

	if(pinPair < 0 || pinPair > 4 || pinPair == 3) {
		return INPUT_ADC_INVALID_PIN;
	}
	if(*isr == 0) {
		return INPUT_ADC_INVALID_ISR;
	}
	if(Input_ADC_callbackFinish[pinPair] == 0) {
		return INPUT_ADC_PIN_INACTIVE;
	}

	Input_ADC_callbackFinish[pinPair] = isr;						//set new ISR

	return INPUT_ADC_SUCCESS;
}

/**
 * interrupt service routine of ADC0 (only used internally)
 */
interrupt void Input_ADC_A0B0_ISR(void) {
	Input_ADC_FillBuffer(0, (float32) AdcResult.ADCRESULT0, (float32) AdcResult.ADCRESULT1);
	AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
	PieCtrlRegs.PIEACK.bit.ACK1 = 1;
}

/**
 * interrupt service routine of ADC1 (only used internally)
 */
interrupt void Input_ADC_A1B1_ISR(void) {
	Input_ADC_FillBuffer(1, (float32) AdcResult.ADCRESULT2, (float32) AdcResult.ADCRESULT3);
	AdcRegs.ADCINTFLGCLR.bit.ADCINT2 = 1;
	PieCtrlRegs.PIEACK.bit.ACK1 = 1;
}

/**
 * interrupt service routine of ADC2 (only used internally)
 */
interrupt void Input_ADC_A2B2_ISR(void) {
	Input_ADC_FillBuffer(2, (float32) AdcResult.ADCRESULT4, (float32) AdcResult.ADCRESULT5);
	AdcRegs.ADCINTFLGCLR.bit.ADCINT3 = 1;
	PieCtrlRegs.PIEACK.bit.ACK10 = 1;
}

/**
 * interrupt service routine of ADC4 (only used internally)
 */
interrupt void Input_ADC_A4B4_ISR(void) {
	Input_ADC_FillBuffer(4, (float32) AdcResult.ADCRESULT6, (float32) AdcResult.ADCRESULT7);
	AdcRegs.ADCINTFLGCLR.bit.ADCINT4 = 1;
	PieCtrlRegs.PIEACK.bit.ACK10 = 1;
}

/**
 * Fills the buffer(s) of an ADC with data (only used internally) and calls the callbackfunction associated with the ADC when
 * the buffer is full.
 * @param pinPair the number of the ADC (0,1,2,4)
 * @param result0 data from ADCxA
 * @param result1 data from ADCxB
 */
void Input_ADC_FillBuffer(int pinPair, float32 result0, float32 result1)
{

	Utilities_SimpleBuffer_In(result0, Input_ADC_buffers[pinPair * 2]);

	if(Input_ADC_buffers[pinPair * 2 + 1]) {
		Utilities_SimpleBuffer_In(result1, Input_ADC_buffers[pinPair * 2 + 1]);
	}
	if(Utilities_SimpleBuffer_Status(Input_ADC_buffers[pinPair * 2]) == UTILITIES_SIMPLE_BUFFER_FULL) {
		float32 *fullBuffer0, *fullBuffer1;
		fullBuffer0 = Utilities_SimpleBuffer_FinalizeSegment(Input_ADC_buffers[pinPair * 2]);
		if(Input_ADC_buffers[pinPair * 2 + 1]) {
			fullBuffer1 = Utilities_SimpleBuffer_FinalizeSegment(Input_ADC_buffers[pinPair * 2 + 1]);
		}
		else {
			fullBuffer1 = 0;
		}
		Input_ADC_callbackFinish[pinPair](fullBuffer0, fullBuffer1);
	}
}

//EOF
