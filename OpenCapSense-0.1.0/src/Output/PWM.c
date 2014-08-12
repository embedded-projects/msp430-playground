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
 * @file PWM.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief This module bundles the PWM output functionality.
 *
 **/

#include "Output/PWM.h"

interrupt void Output_PWM_EPWM1ISR();

void Output_PWM_Init() {
	DINT;

	// Interrupts that are used in this example are re-mapped to
	// ISR functions found within this file.
	EALLOW;  // This is needed to write to EALLOW protected registers
	PieVectTable.EPWM1_INT = &Output_PWM_EPWM1ISR;
	EDIS;    // This is needed to disable write to EALLOW protected registers

	// Enable CPU INT3 which is connected to EPWM1-3 INT:
	IER |= M_INT3;

	// Enable EPWM INTn in the PIE: Group 3 interrupt 1-3
	PieCtrlRegs.PIEIER3.bit.INTx1 = 1;
	PieCtrlRegs.PIEIER3.bit.INTx2 = 1;
	PieCtrlRegs.PIEIER3.bit.INTx3 = 1;

	// Enable global Interrupts and higher priority real-time debug events:
	EINT;   // Enable Global interrupt INTM
	ERTM;   // Enable Global realtime interrupt DBGM
}

/**
 * initializes a PWM for a given Pin
 */

void Output_PWM_InitGpio(int gpioNumber, int counter) {

	//Sends the signal trough CH1 and CH2
	//Uses the PWM1
	if(gpioNumber == 1 || gpioNumber == 0) {

		EALLOW;
			SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
		EDIS;

		InitEPwm1Gpio();

		EALLOW;
	    	GpioCtrlRegs.GPAPUD.bit.GPIO1 = 1;    	// Disable pull-up on GPIO0 (EPWM1A)
	    	//GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;	//configure as output
	    	GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 1; 	// Configure GPIO0 as EPWM1A
	    EDIS;

	    // Setup TBCLK
	    EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN;	 // Count up
	    EPwm1Regs.TBPRD = counter;      				 // Set timer period
	    EPwm1Regs.TBCTL.bit.PHSEN = TB_DISABLE;   		 // Disable phase loading
	    EPwm1Regs.TBPHS.half.TBPHS = 0x0000;     		 // Phase is 0
	    EPwm1Regs.TBCTR = 0x0000;               		 // Clear counter
	    EPwm1Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   		 // Clock ratio to SYSCLKOUT
	    EPwm1Regs.TBCTL.bit.CLKDIV = TB_DIV2;

	    // Setup shadow register load on ZERO
	    EPwm1Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	    EPwm1Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
	    EPwm1Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	    EPwm1Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

	    // Set Compare values
	    EPwm1Regs.CMPA.half.CMPA = counter;    // Set compare A value, 425
	    EPwm1Regs.CMPB = counter;              // Set Compare B value

	    // Set actions
	    EPwm1Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM1A on Zero
	    EPwm1Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM1A on event A, up count

	    EPwm1Regs.AQCTLB.bit.ZRO = AQ_SET;            // Set PWM1B on Zero
	    EPwm1Regs.AQCTLB.bit.CBU = AQ_CLEAR;          // Clear PWM1B on event B, up count

	    // Interrupt where we will change the Compare Values
	    EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_ZERO;     // Select INT on Zero event
	    EPwm1Regs.ETSEL.bit.INTEN = 1;                // Enable INT
	    EPwm1Regs.ETPS.bit.INTPRD = ET_3RD;           // Generate INT on 3rd event

		EALLOW;
		SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
		EDIS;
	}

	//Sends the signal trough CH3
	//Uses the PWM2
	if(gpioNumber==2){
		EALLOW;
			SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
		EDIS;

		InitEPwm2Gpio();

		EALLOW;
		GpioCtrlRegs.GPAPUD.bit.GPIO2 = 1;    // Disable pull-up on GPIO2 (EPWM2A)
		GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 1;   // Configure GPIO2 as EPWM2A

		EDIS;

	    // Setup TBCLK (= Time Base Control Register)
	    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
	    EPwm2Regs.TBPRD = counter;       // Set timer period
	    EPwm2Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
	    EPwm2Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
	    EPwm2Regs.TBCTR = 0x0000;                  // Clear counter
	    EPwm2Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
	    EPwm2Regs.TBCTL.bit.CLKDIV = TB_DIV2;

	    // Setup shadow register load on ZERO
	    EPwm2Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
	    EPwm2Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
   	    EPwm2Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
	    EPwm2Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

   	    // Set Compare values
   	    EPwm2Regs.CMPA.half.CMPA = counter;   	// Set compare A value
   	    EPwm2Regs.CMPB = counter;				// Set Compare B value

   	 //Set actions
   	 	EPwm2Regs.AQCTLA.bit.ZRO = AQ_SET;  	//Set PWM3A on Zero
   	 	EPwm2Regs.AQCTLA.bit.CAU = AQ_CLEAR; 	//Clear PWM3A on event A, up count
		EPwm2Regs.AQCTLB.bit.ZRO = AQ_SET; 		//Set PWM3B on Zero
		EPwm2Regs.AQCTLB.bit.CBU = AQ_CLEAR; 	//Clear PWM3B on event B, up count

   		EALLOW;
   		SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
   		EDIS;

	}

	//Sends the signal trough CH5
	//Uses the PWM3
	if(gpioNumber==4){

		EALLOW;
			SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
		EDIS;

		InitEPwm3Gpio();

		EALLOW;
		  	GpioCtrlRegs.GPAPUD.bit.GPIO4 = 1;    // Disable pull-up on GPIO0 (EPWM1A)
			GpioCtrlRegs.GPAMUX1.bit.GPIO4 = 1;   // Configure GPIO0 as EPWM1A
		EDIS;

			    // Setup TBCLK
			    EPwm3Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; // Count up
			    EPwm3Regs.TBPRD = counter;       // Set timer period
			    EPwm3Regs.TBCTL.bit.PHSEN = TB_DISABLE;    // Disable phase loading
			    EPwm3Regs.TBPHS.half.TBPHS = 0x0000;       // Phase is 0
			    EPwm3Regs.TBCTR = 0x0000;                  // Clear counter
			    EPwm3Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2;   // Clock ratio to SYSCLKOUT
			    EPwm3Regs.TBCTL.bit.CLKDIV = TB_DIV2;

			    // Setup shadow register load on ZERO
			    EPwm3Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
			    EPwm3Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
			    EPwm3Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
			    EPwm3Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

			    // Set Compare values

			    EPwm3Regs.CMPA.half.CMPA = counter;    // Set compare A value, 425
			    EPwm3Regs.CMPB = counter;              // Set Compare B value

			    // Set actions
			    EPwm3Regs.AQCTLA.bit.ZRO = AQ_SET;            // Set PWM1A on Zero
			    EPwm3Regs.AQCTLA.bit.CAU = AQ_CLEAR;          // Clear PWM1A on event A, up count

			    EPwm3Regs.AQCTLB.bit.ZRO = AQ_SET;            // Set PWM1B on Zero
			    EPwm3Regs.AQCTLB.bit.CBU = AQ_CLEAR;          // Clear PWM1B on event B, up count


				EALLOW;
				SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
				EDIS;

	}

	//Sends the signal trough CH7
	//Uses the PWM4
	if(gpioNumber == 6){
		EALLOW;
			SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
		EDIS;

		InitEPwm4Gpio();

		EALLOW;
			GpioCtrlRegs.GPAPUD.bit.GPIO6 = 1;
			GpioCtrlRegs.GPAMUX1.bit.GPIO6 = 1;
		EDIS;

		EPwm4Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; //Count up
		EPwm4Regs.TBPRD = counter; //Set timer period
		EPwm4Regs.TBCTL.bit.PHSEN = TB_DISABLE;
		EPwm4Regs.TBPHS.half.TBPHS = 0x0000; //Phase is 0
		EPwm4Regs.TBCTR = 0x0000; //clear contuer
		EPwm4Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2; // Clock Ratio to SYSCLKOUT
		EPwm4Regs.TBCTL.bit.CLKDIV = TB_DIV2;

		//Setup shadow register load on ZERO
		EPwm4Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
		EPwm4Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
		EPwm4Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
		EPwm4Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

		//Set compare values
		EPwm4Regs.CMPA.half.CMPA = counter; //set compare A value
		EPwm4Regs.CMPB = counter;			//set compare B value

		//Set actions
		EPwm4Regs.AQCTLA.bit.ZRO = AQ_SET;  	//Set PWM4A on Zero
		EPwm4Regs.AQCTLA.bit.CAU = AQ_CLEAR; 	//Clear PWM4A on event A, up count

		EPwm4Regs.AQCTLB.bit.ZRO = AQ_SET; 		//Set PWM4B on Zero
		EPwm4Regs.AQCTLB.bit.CBU = AQ_CLEAR; 	//Clear PWM4B on event B, up count

		EALLOW;
			SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
		EDIS;
	}

	//Sends the signal trough CH6
	//Uses the PWM5
	if(gpioNumber == 8){
		EALLOW;
			SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
		EDIS;

		InitEPwm5Gpio();

		EALLOW;
			GpioCtrlRegs.GPAPUD.bit.GPIO8 = 1;
			GpioCtrlRegs.GPAMUX1.bit.GPIO8 = 1;
		EDIS;

		EPwm5Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; //Count up
		EPwm5Regs.TBPRD = counter; //Set timer period
		EPwm5Regs.TBCTL.bit.PHSEN = TB_DISABLE;
		EPwm5Regs.TBPHS.half.TBPHS = 0x0000; //Phase is 0
		EPwm5Regs.TBCTR = 0x0000; //clear contuer
		EPwm5Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2; // Clock Ratio to SYSCLKOUT
		EPwm5Regs.TBCTL.bit.CLKDIV = TB_DIV2;

		//Setup shadow register load on ZERO
		EPwm5Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
		EPwm5Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
		EPwm5Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
		EPwm5Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

		//Set compare values
		EPwm5Regs.CMPA.half.CMPA = counter; //set compare A value
		EPwm5Regs.CMPB = counter;			//set compare B value
		//Set actions
		EPwm5Regs.AQCTLA.bit.ZRO = AQ_SET;  	//Set PWM5A on Zero
		EPwm5Regs.AQCTLA.bit.CAU = AQ_CLEAR; 	//Clear PWM5A on event A, up count

		EPwm5Regs.AQCTLB.bit.ZRO = AQ_SET; 		//Set PWM5B on Zero
		EPwm5Regs.AQCTLB.bit.CBU = AQ_CLEAR; 	//Clear PWM5B on event B, up count

		EALLOW;
			SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
		EDIS;

	}

	//Sends the signal trough CH8
	//Uses the PWM6
	if(gpioNumber == 10){
			EALLOW;
				SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
			EDIS;

		InitEPwm6Gpio();

			EALLOW;
				GpioCtrlRegs.GPAPUD.bit.GPIO10 = 1;
				GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;
			EDIS;

			EPwm6Regs.TBCTL.bit.CTRMODE = TB_COUNT_UPDOWN; //Count up
			EPwm6Regs.TBPRD = counter; //Set timer period
			EPwm6Regs.TBCTL.bit.PHSEN = TB_DISABLE;
			EPwm6Regs.TBPHS.half.TBPHS = 0x0000; //Phase is 0
			EPwm6Regs.TBCTR = 0x0000; //clear counter
			EPwm6Regs.TBCTL.bit.HSPCLKDIV = TB_DIV2; // Clock Ratio to SYSCLKOUT
			EPwm6Regs.TBCTL.bit.CLKDIV = TB_DIV2;

			//Setup shadow register load on ZERO
			EPwm6Regs.CMPCTL.bit.SHDWAMODE = CC_SHADOW;
			EPwm6Regs.CMPCTL.bit.SHDWBMODE = CC_SHADOW;
			EPwm6Regs.CMPCTL.bit.LOADAMODE = CC_CTR_ZERO;
			EPwm6Regs.CMPCTL.bit.LOADBMODE = CC_CTR_ZERO;

			//Set compare values
			EPwm6Regs.CMPA.half.CMPA = counter; //set compare A value
			EPwm6Regs.CMPB = counter;			//set compare B value

			//Set actions
			EPwm6Regs.AQCTLA.bit.ZRO = AQ_SET;  	//Set PWM6A on Zero
			EPwm6Regs.AQCTLA.bit.CAU = AQ_CLEAR; 	//Clear PWM6A on event A, up count

			EPwm6Regs.AQCTLB.bit.ZRO = AQ_SET; 		//Set PWM6B on Zero
			EPwm6Regs.AQCTLB.bit.CBU = AQ_CLEAR; 	//Clear PWM6B on event B, up count

			EALLOW;
				SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
			EDIS;
		}

}

/**
 * resets the pwm to have a clean phase on start of some operation
 * @param the number of gpio
 */
Output_PWM_Reset(int gpioNumber) {
	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	EDIS;
	if(gpioNumber == 0 || gpioNumber == 1){
	    EPwm1Regs.TBCTR = 0x0000;                  // Clear counter
	}
	if(gpioNumber == 2) {
	    EPwm2Regs.TBCTR = 0x0000;                  // Clear counter
	}
	if(gpioNumber == 4){
		EPwm3Regs.TBCTR = 0x0000;				//Clear counter
	}

	if(gpioNumber == 3 || gpioNumber == 7){
		EPwm4Regs.TBCTR = 0x0000;				//Clear counter
	}
	if(gpioNumber == 8){
		EPwm5Regs.TBCTR = 0x0000;				//Clear counter
	}

	if(gpioNumber == 10){
		EPwm6Regs.TBCTR = 0x0000;				//Clear counter
	}
	if(gpioNumber == 30){
		EPwm7Regs.TBCTR = 0x0000;				//Clear counter
	}


	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;
	EDIS;
}


/**
 * ISR for EPWM 1
 */
interrupt void Output_PWM_EPWM1ISR() {
	// Clear INT flag for this timer
	EPwm1Regs.ETCLR.bit.INT = 1;

	// Acknowledge this interrupt to receive more interrupts from group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

//EOF
