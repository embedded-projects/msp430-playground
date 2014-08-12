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
 * @brief This module initializes the mcu registers for the components being used.
 * This configuration has been adopted from the f28069 Software examples
 *
 **/

#include "Device/Init.h"

int Device_Init_resetFlag = 0;
int Device_Init_watchdogCount = 0;

void Device_Init_Flash(void);
void Device_Init_MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr);
interrupt void Device_Init_WatchdogISR(void);

// Functions that will be run from RAM need to be assigned to
// a different section.  This section will then be mapped to a load and
// run address using the linker cmd file.
#pragma CODE_SECTION(Device_Init_Flash, "ramfuncs");

extern Uint16 RamfuncsLoadStart, RamfuncsLoadEnd, RamfuncsRunStart; /**<  Used for running BackGround in flash, and ISR in RAM */

/**
 * initializes the device
 **/
void Device_Init() {

	// Step 1. Initialize System Control:
	// PLL, WatchDog, enable Peripheral Clocks
	// This example function is found in the F2806x_SysCtrl.c file.
	// Disable the watchdog
	DisableDog();

	// *IMPORTANT*
	// The Device_cal function, which copies the ADC & oscillator calibration values
	// from TI reserved OTP into the appropriate trim registers, occurs automatically
	// in the Boot ROM. If the boot ROM code is bypassed during the debug process, the
	// following function MUST be called for the ADC and oscillators to function according
	// to specification. The clocks to the ADC MUST be enabled before calling this
	// function.
	// See the device data manual and/or the ADC Reference
	// Manual for more information.

	EALLOW;
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 1; // Enable ADC peripheral clock
	(*Device_cal)();
	SysCtrlRegs.PCLKCR0.bit.ADCENCLK = 0; // Return ADC clock to original state
	EDIS;

	#if DEVICE_INIT_REVISION > 2
		ExtOscSel();

		// Initialize the PLL control: PLLCR and CLKINDIV
		// DSP28_PLLCR and DSP28_CLKINDIV are defined in F2806x_Examples.h
		InitPll(8, DSP28_DIVSEL);

		// Initialize the peripheral clocks
		InitPeripheralClocks();
	#else
		// Select Internal Oscillator 1 as Clock Source (default), and turn off all unused clocks to
		// conserve power.
		IntOsc1Sel();

		// Initialize the PLL control: PLLCR and CLKINDIV
		// DSP28_PLLCR and DSP28_CLKINDIV are defined in F2806x_Examples.h
		InitPll(DSP28_PLLCR,DSP28_DIVSEL);

		// Initialize the peripheral clocks
		InitPeripheralClocks();
	#endif


	// Step 2. Initialize GPIO:
	// This example function is found in the F2806x_Gpio.c file and
	// illustrates how to set the GPIO to it's default state.
	// InitGpio();  // Skipped for this example
	// Copy time critical code and Flash setup code to RAM
	// The  RamfuncsLoadStart, RamfuncsLoadEnd, and RamfuncsRunStart
	// symbols are created by the linker. Refer to the linker files. 
	Device_Init_MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);

	// Call Flash Initialization to setup flash waitstates
	// This function must reside in RAM
	Device_Init_Flash();	// Call the flash wrapper init function

	// initialize all cpu timers
	InitCpuTimers();

	DINT;

	// Step 3. Clear all interrupts and initialize PIE vector table:
	// Disable CPU interrupts
	//DINT;
	
	// Initialize the PIE control registers to their default state.
	// The default state is all PIE interrupts disabled and flags
	// are cleared.
	// This function is found in the F2806x_PieCtrl.c file.
	InitPieCtrl();
	
	// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;
	
	// Initialize the PIE vector table with pointers to the shell Interrupt
	// Service Routines (ISR).
	// This will populate the entire table, even if the interrupt
	// is not used in this example.  This is useful for debug purposes.
	// The shell ISR routines are found in F2806x_DefaultIsr.c.
	// This function is found in F2806x_PieVect.c.
	InitPieVectTable();

	// Initialize sub modules

	// Step 4. Initialize all the Device Peripherals:
	// This function is found in F2806x_InitPeripherals.c
	// InitPeripherals(); // Not required for this example
	EINT;
	ERTM;

	InitCpuTimers();
}

/**
 * initializes the external clock source at pin X
 */
void Device_Init_ExternalClockSource() {
    EALLOW;
    SysCtrlRegs.XCLK.bit.XCLKINSEL = 1;       // 1-GPIO19 = XCLKIN, 0-GPIO38 = XCLKIN
    SysCtrlRegs.CLKCTL.bit.XTALOSCOFF = 1;    // Turn on XTALOSC
    SysCtrlRegs.CLKCTL.bit.XCLKINOFF = 0;     // Turn on XCLKIN
    SysCtrlRegs.CLKCTL.bit.OSCCLKSRC2SEL = 0; // Switch to external clock
    SysCtrlRegs.CLKCTL.bit.OSCCLKSRCSEL = 1;  // Switch from INTOSC1 to INTOSC2/ext clk
    SysCtrlRegs.CLKCTL.bit.WDCLKSRCSEL = 0;   // Clock Watchdog off of INTOSC1 always
    SysCtrlRegs.CLKCTL.bit.INTOSC2OFF = 1;    // Turn off INTOSC2
    SysCtrlRegs.CLKCTL.bit.INTOSC1OFF = 0;     // Leave INTOSC1 on
    EDIS;
}

/**
 * initializes flash memory, must be executed in ram
 **/
void Device_Init_Flash(void) {
   EALLOW;
   //Enable Flash Pipeline mode to improve performance
   //of code executed from Flash.
   FlashRegs.FOPT.bit.ENPIPE = 1;

   //                CAUTION
   //Minimum waitstates required for the flash operating
   //at a given CPU rate must be characterized by TI.
   //Refer to the datasheet for the latest information.

   //Set the Paged Waitstate for the Flash
   FlashRegs.FBANKWAIT.bit.PAGEWAIT = 3;

   //Set the Random Waitstate for the Flash
   FlashRegs.FBANKWAIT.bit.RANDWAIT = 3;

   //Set the Waitstate for the OTP
   FlashRegs.FOTPWAIT.bit.OTPWAIT = 1;

   //                CAUTION
   //ONLY THE DEFAULT VALUE FOR THESE 2 REGISTERS SHOULD BE USED
   FlashRegs.FSTDBYWAIT.bit.STDBYWAIT = 0x01FF;
   FlashRegs.FACTIVEWAIT.bit.ACTIVEWAIT = 0x01FF;
   EDIS;

   //Force a pipeline flush to ensure that the write to
   //the last register configured occurs before returning.

   // Set the flash OTP wait-states to minimum. This is important
   // for the performance of the temperature conversion function.
   //FlashRegs.FOTPWAIT.bit.OTPWAIT = 1;
   asm(" RPT #7 || NOP");
}

/**
 * This function will copy the specified memory contents from
 * one location to another. 
 * @param Uint16 *SourceAddr - Pointer to the first word to be moved SourceAddr < SourceEndAddr
 * @param Uint16* SourceEndAddr - Pointer to the last word to be moved
 * @param Uint16* DestAddr - Pointer to the first destination word
 * 
 * No checks are made for invalid memory locations or that the
 * end address is > then the first start address.
 **/
void Device_Init_MemCopy(Uint16 *SourceAddr, Uint16* SourceEndAddr, Uint16* DestAddr) {
    while(SourceAddr < SourceEndAddr) { 
       *DestAddr++ = *SourceAddr++;
    }
    return;
}

/**
 * initializes the watchdog
 */
void Device_Init_Watchdog() {
	// Enable the watchdog
	EALLOW;
	SysCtrlRegs.WDCR = 0x0028;
	//SysCtrlRegs.WDCR |= BIT2; // set prescaler to div 4
	EDIS;

	// Reset the watchdog counter
	ServiceDog();
}

// EOF
