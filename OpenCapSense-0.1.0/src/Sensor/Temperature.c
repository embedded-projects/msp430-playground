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
 * @file Temperature.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief Samples the MCU's internal temperature sensor. Used for drift compensation
 * when no external oscillator is present.
 *
 **/

#include "Sensor/Temperature.h"

interrupt void Sensor_Temperature_ISR(void);

volatile int Sensor_Temperature_updated = 0;
int Sensor_Temperature_result = 0;

int Sensor_Temperature_Init() {
	InitAdc();

	EALLOW;
	AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 1;	//Enable non-overlap mode

	AdcRegs.ADCCTL1.bit.TEMPCONV  = 1; 		//Connect channel A5 internally to the temperature sensor
	AdcRegs.ADCSOC0CTL.bit.CHSEL  = 5; 		//Set SOC0 channel select to ADCINA5
	AdcRegs.ADCSOC0CTL.bit.ACQPS  = 63; 		//Set SOC0 acquisition period to 26 ADCCLK

	AdcRegs.INTSEL1N2.bit.INT1E     = 1;	//Enabled ADCINT1
	AdcRegs.INTSEL1N2.bit.INT1CONT  = 0;	//Disable ADCINT1 Continuous mode
	AdcRegs.INTSEL1N2.bit.INT1SEL	= 0;	//setup EOC0 to trigger ADCINT1 to fire

	PieCtrlRegs.PIEIER1.bit.INTx1 = 1;				//enable ADC-interrupts
	PieVectTable.ADCINT1 = &Sensor_Temperature_ISR;

	// Enable ADCINT1 in PIE
	IER |= M_INT1; 						// Enable CPU Interrupt 1

	EDIS;
}

int16 Sensor_Temperature_GetValue() {
	EALLOW;
	//Force start of conversion on SOC0
	AdcRegs.ADCSOCFRC1.all = 0x01;
	EDIS;

	//Wait for end of conversion.
	while(Sensor_Temperature_updated == 0) {}  //Wait for ADCINT1
	Sensor_Temperature_updated = 0;

	return Sensor_Temperature_result;
}

/**
 * interrupt service routine of ADC1 (only used internally)
 */
interrupt void Sensor_Temperature_ISR(void) {
	AdcRegs.ADCINTFLGCLR.bit.ADCINT1 = 1;
	PieCtrlRegs.PIEACK.bit.ACK1 = 1;

	Sensor_Temperature_updated = 1;
	Sensor_Temperature_result = AdcResult.ADCRESULT0;
}

// EOF
