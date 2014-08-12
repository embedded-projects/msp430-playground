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
 * @file ECAP.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief This module bundles the ECAP functionality. Note that switches must be
 * configured to switch to ECAP pins before usage (Device_Switch_*)
 *
 **/

#include "Input/ECAP.h"

volatile struct ECAP_REGS *Input_ECAP_ecapRegs[3] = {&ECap1Regs, &ECap2Regs, &ECap3Regs}; /**< the ecap regs as an array */
int Input_ECAP_currentSensor[3] = {-1, -1, -1}; /**< currrent pins */
float Input_ECAP_results[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int Input_ECAP_valueCount[8] = {0, 0, 0, 0, 0, 0, 0, 0};
int Input_ECAP_dismissCycles = INPUT_ECAP_DISMISSCYCLES;

/**
 * Initializes a new eCap at the given pinNumber
 *
 * @param the number of the GPIO associated with the eCap
 * 		Ecap1 = GPIO5 = Pin8
 * 		Ecap2 = GPIO24 = Pin77
 * 		Ecap3 = GPIO15 = Pin70
 * 		Ecap4 = GPIO26 = Pin62
 * 		Ecap5 = GPIO11 = Pin59
 * 		Ecap6 = GPIO19 = Pin52
 * 		Ecap7 = GPIO7 = Pin45
 * 		Ecap8 = GPIO9 = Pin39
 * 		Ecap6 = GPIO25 = Pinx // new ecap
 * @param buffer pointer to an initialized Utilities_SimpleBuffer struct which will be filled with data
 * @param isr function pointer to the interrupt service routine which is called when the buffer is full
 * 		the first parameter is an array of floats (the BufferSegment)
 * 		the second parameter is the eCap whose Buffer was full (between 1 and 8)
 * @return an error code or 0 if successful
 */
int Input_ECAP_Init(int gpioNumber) {
	int sensorNumber = Input_ECAP_GetSensorNumberForGPIO(gpioNumber);
	volatile struct ECAP_REGS *ecapRegs;

	if(sensorNumber == -1) {
		return INPUT_ECAP_INVALID_ECAP;
	}

	ecapRegs = Input_ECAP_ecapRegs[Input_ECAP_GetEcapNumber(gpioNumber)];
	IER |= M_INT4;								//enable eCap-interrupts

	EALLOW;
	PieVectTable.ECAP1_INT = &Input_ECAP_ECAP1_ISR;
	PieVectTable.ECAP2_INT = &Input_ECAP_ECAP2_ISR;
	PieVectTable.ECAP3_INT = &Input_ECAP_ECAP3_ISR;
	EDIS;

	ecapRegs->ECCTL2.bit.CONT_ONESHT = 1;		//One-shot
	ecapRegs->ECCTL2.bit.STOP_WRAP = 3;       	//Stop at 4 events
	ecapRegs->ECCTL1.bit.CAP1POL = 0;			//Rising edge
	ecapRegs->ECCTL1.bit.CAP2POL = 0;        	//Rising edge
	ecapRegs->ECCTL1.bit.CAP3POL = 0;			//Rising edge
	ecapRegs->ECCTL1.bit.CAP4POL = 0;          //Rising edge
	ecapRegs->ECCTL1.bit.CTRRST1 = 1;          //Difference operation
	ecapRegs->ECCTL1.bit.CTRRST2 = 1;          //Difference operation
	ecapRegs->ECCTL1.bit.CTRRST3 = 1;          //Difference operation
	ecapRegs->ECCTL1.bit.CTRRST4 = 1;          //Difference operation
	ecapRegs->ECCTL2.bit.SYNCI_EN = 1;         //Enable sync in
	ecapRegs->ECCTL2.bit.SYNCO_SEL = 0;        //Pass through
	ecapRegs->ECCTL1.bit.CAPLDEN = 1;          //Enable capture units
	ecapRegs->ECCTL2.bit.TSCTRSTOP = 0;        //Start Counter
	ecapRegs->ECCTL2.bit.REARM = 1;            //arm one-shot
	ecapRegs->ECCTL1.bit.CAPLDEN = 1;          //Enable CAP1-CAP4 register loads
	ecapRegs->ECEINT.bit.CEVT4 = 1;            //4 events = interrupt
	ecapRegs->ECEINT.bit.CTROVF = 0; 			// counter overlfow IR enable
	ecapRegs->ECCTL1.bit.PRESCALE = 0x0;

	return INPUT_ECAP_SUCCESS;
}

/**
 * uninitializes an eCap for further use
 * @param gpioNumber the eCap which shall be deactivated
 * @return returns an error code or 0 if successful
 */
int Input_ECAP_Uninit(int gpioNumber) {
	volatile struct ECAP_REGS *ecapRegs;
	int sensorNumber = Input_ECAP_GetSensorNumberForGPIO(gpioNumber);
	int ecapNumber = Input_ECAP_GetEcapNumber(gpioNumber);
	ecapRegs = Input_ECAP_ecapRegs[ecapNumber];

	EALLOW;
	if(Input_ECAP_currentSensor[ecapNumber] == sensorNumber) {
		Input_ECAP_currentSensor[ecapNumber] = -1;
		ecapRegs->ECCTL2.bit.TSCTRSTOP = 0;        //stop Counter
	}
	EDIS;

	return INPUT_ECAP_SUCCESS;
}

/**
 * switches the pin of an eCap
 * @param gpioNumber the GPIO which shall be activated. All other GPIOs of the same eCap will be disabled
 * @return returns an error code or 0 if successful
 */
int Input_ECAP_SwitchPin(int gpioNumber) {
	EALLOW;
	if(gpioNumber == 5) {							//eCap 1
		GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0;
		GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;
		GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;
		GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 3;
		Input_ECAP_currentSensor[0] = 0;
	} else if(gpioNumber == 24) {						//eCap 1
		GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;
		GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;
		GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;
		GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 1;
		Input_ECAP_currentSensor[0] = 1;
	} else if(gpioNumber == 15) {						//eCap 2
		GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;
		GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 1;
		GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;
		Input_ECAP_currentSensor[1] = 2;
	} else if(gpioNumber == 26) {					//eCap 3
		GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 0;
		GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 1;
		Input_ECAP_currentSensor[2] = 3;
	} else if(gpioNumber == 11) {				//eCap 1
		GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;
		GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0;
		GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;
		GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 3;
		Input_ECAP_currentSensor[0] = 4;
	} else if(gpioNumber == 19) {						//eCap 1
		GpioCtrlRegs.GPAMUX1.bit.GPIO5 = 0;
		GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0;
		GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 0;
		GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 3;
		Input_ECAP_currentSensor[0] = 5;
	} else if(gpioNumber == 7) {						//eCap 2
		GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;
		GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 3;
		GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 0;
		Input_ECAP_currentSensor[1] = 6;
	} else if(gpioNumber == 9) {						//eCap 3
		GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 0;
		GpioCtrlRegs.GPAMUX1.bit.GPIO9 = 3;
		Input_ECAP_currentSensor[2] = 7;
	} else if(gpioNumber == 25) {						//eCap 2
		GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;
		GpioCtrlRegs.GPAMUX1.bit.GPIO7 = 0;
		GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 1;
		Input_ECAP_currentSensor[1] = 5;
	}
	EDIS;

	return INPUT_ECAP_SUCCESS;
}

/**
 * enables the specified eCap
 * @param gpioNumber of the eCap which should be enabled
 * @return returns an error code or 0 if successful
 */
int Input_ECAP_Enable(int gpioNumber) {
	volatile struct ECAP_REGS *ecapRegs;
	int sensorNumber = Input_ECAP_GetSensorNumberForGPIO(gpioNumber);
	int ecapNumber = Input_ECAP_GetEcapNumber(gpioNumber);
	ecapRegs = Input_ECAP_ecapRegs[ecapNumber];

	if(sensorNumber == -1) {
		return INPUT_ECAP_INVALID_ECAP;
	}

	Input_ECAP_valueCount[sensorNumber] = 0;
	Input_ECAP_results[sensorNumber] = 0;

	Input_ECAP_SwitchPin(gpioNumber);
	Input_ECAP_EnableInterrupt(ecapNumber);
	Input_ECAP_currentSensor[ecapNumber] = sensorNumber;

	ecapRegs->ECCTL2.bit.TSCTRSTOP = 1; //start counter

	return INPUT_ECAP_SUCCESS;
}

/**
 * disables the specified eCap
 * @param gpioNumber the eCap which should be disabled
 * @return returns an error code or 0 if successful
 */
int Input_ECAP_Disable(int gpioNumber) {
	volatile struct ECAP_REGS *ecapRegs;
	int ecapNumber = Input_ECAP_GetEcapNumber(gpioNumber);
	int sensorNumber = Input_ECAP_GetSensorNumberForGPIO(gpioNumber);

	ecapRegs = Input_ECAP_ecapRegs[ecapNumber];

	EALLOW;
	ecapRegs->ECCTL2.bit.TSCTRSTOP = 0;

	Input_ECAP_DisableInterrupt(ecapNumber);
	Input_ECAP_currentSensor[ecapNumber] = -1;
	EDIS;

	return INPUT_ECAP_SUCCESS;
}

/**
 * sets the dismiss cycles for measuring
 */
void Input_ECAP_SetDismissCycles(int number) {
	if(number < 0 || number > 1000) {
		return;
	}

	Input_ECAP_dismissCycles = number;
}

/**
 * disables the interrupt for a given ecap number
 * @param ecap number
 */
void Input_ECAP_DisableInterrupt(int ecapNumber) {
	EALLOW;
	if(ecapNumber == 0) {
		PieCtrlRegs.PIEIER4.bit.INTx1 = 0;
	} else if(ecapNumber == 1) {
		PieCtrlRegs.PIEIER4.bit.INTx2 = 0;
	} else if(ecapNumber == 2) {
		PieCtrlRegs.PIEIER4.bit.INTx3 = 0;
	}
	EDIS;
}

/**
 * disables the interrupt for a given ecap number
 * @param ecap number
 */
void Input_ECAP_EnableInterrupt(int ecapNumber) {
	EALLOW;
	if(ecapNumber == 0) {
		PieCtrlRegs.PIEIER4.bit.INTx1 = 1;
	} else if(ecapNumber == 1) {
		PieCtrlRegs.PIEIER4.bit.INTx2 = 1;
	} else if(ecapNumber == 2) {
		PieCtrlRegs.PIEIER4.bit.INTx3 = 1;
	}
	EDIS;
}

/**
 * interrupt service routine of physical eCap1 (only used internally)
 */
interrupt void Input_ECAP_ECAP1_ISR(void) {
	Input_ECAP_SaveResult(0);
}

/**
 * interrupt service routine of physical eCap2 (only used internally)
 */
interrupt void Input_ECAP_ECAP2_ISR(void) {
	Input_ECAP_SaveResult(1);
}

/**
 * interrupt service routine of physical eCap3 (only used internally)
 */
interrupt void Input_ECAP_ECAP3_ISR(void) {
	Input_ECAP_SaveResult(2);
}

/**
 * Fills the data buffers of each eCap in use and handles the switching of pins (only used internally).
 * The pin is switched when the buffer is full.
 * @param isrNumber physical eCap number (between 0 and 2)
 */
void Input_ECAP_SaveResult(int ecapNumber) {
	int sensorNumber = Input_ECAP_GetSensorNumberForECAP(ecapNumber);

	if(sensorNumber >= 0) {
		float32 result = ((float32) (Input_ECAP_ecapRegs[ecapNumber]->CAP1 + Input_ECAP_ecapRegs[ecapNumber]->CAP2 + Input_ECAP_ecapRegs[ecapNumber]->CAP3 + Input_ECAP_ecapRegs[ecapNumber]->CAP4)) / 4;

		Input_ECAP_valueCount[sensorNumber] = Input_ECAP_valueCount[sensorNumber] + 1;

		if(Input_ECAP_valueCount[sensorNumber] > Input_ECAP_dismissCycles) {
			Input_ECAP_results[sensorNumber] = Input_ECAP_results[sensorNumber] + result;
		}

		Input_ECAP_ecapRegs[ecapNumber]->ECCLR.bit.CEVT4 = 1;
		Input_ECAP_ecapRegs[ecapNumber]->ECCLR.bit.INT = 1;
		Input_ECAP_ecapRegs[ecapNumber]->ECCTL2.bit.REARM = 1;
	}

	// Acknowledge this interrupt to receive more interrupts from group 4
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;
}

/**
 * translates gpioNumbers into pinNumbers (only used internally)
 * @param gpioNumber the gpioNumber
 * @return the pinNumber (-1 if invalid gpio)
 */
int Input_ECAP_GetSensorNumberForGPIO(int gpioNumber) {
	if(gpioNumber == 5) {
		return 0;
	} else if(gpioNumber == 24) {
		return 1;
	} else if(gpioNumber == 15) {
		return 2;
	} else if(gpioNumber == 26) {
		return 3;
	} else if(gpioNumber == 11) {
		return 4;
	} else if(gpioNumber == 19) {
		return 5;
	} else if(gpioNumber == 7) {
		return 6;
	} else if(gpioNumber == 9) {
		return 7;
	} else if(gpioNumber == 25) {
		return 5;
	}

	return -1;
}

/**
 * returns the last measurement count and subtracts the dismiss cycles
 * @param the sensor id (0-7)
 * @return the average number of cycles. note that the average time is number of cycles * 12.5ns
 */
int Input_ECAP_GetValueCountWithoutDismissCycles(int sensorNumber) {
	int result;

	if(sensorNumber < 0 || sensorNumber > 7) {
		return -1;
	}

	result = Input_ECAP_valueCount[sensorNumber] - Input_ECAP_dismissCycles;

	if(result >= 0) {
		return result;
	} else {
		return 0;
	}
}

/**
 * clears the result from count
 * @param the sensor id (0-7)
 */
void Input_ECAP_ClearResult(int sensorNumber) {
	Input_ECAP_valueCount[sensorNumber] = 0;
	Input_ECAP_results[sensorNumber] = 0;
}

/**
 * returns the last measurement result
 * @param the sensor id (0-7)
 */
float Input_ECAP_GetResult(int sensorNumber) {
	if(sensorNumber < 0 || sensorNumber > 7) {
		return -1;
	}

	if(Input_ECAP_valueCount[sensorNumber] < 5) {
		return 0;
	}

	return Input_ECAP_results[sensorNumber];
}

/**
 * translates gpioNumbers into pinNumbers (only used internally)
 * @param gpioNumber the gpioNumber
 * @return the pinNumber (-1 if invalid gpio)
 */
int Input_ECAP_GetSensorNumberForECAP(int ecapNumber) {
	if(ecapNumber < 0 || ecapNumber > 2) {
		return -2;
	}

	return Input_ECAP_currentSensor[ecapNumber];
}

/**
 * returns the ecap register number
 * @param the gpio number
 * @return the corresponding ecap number
 */
int Input_ECAP_GetEcapNumber(int gpioNumber) {
	if(gpioNumber == 5 || gpioNumber == 11 || gpioNumber == 19 || gpioNumber == 24) {			//R.I.P. toller switch-case-block...
		return 0;
	} else if(gpioNumber == 7 || gpioNumber == 15 || gpioNumber == 25) {
		return 1;
	} else if(gpioNumber == 9 || gpioNumber == 26) {
		return 2;
	}

	return -1;
}

//EOF
