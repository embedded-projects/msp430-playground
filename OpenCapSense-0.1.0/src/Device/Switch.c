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
 * @file Switch.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief There are two switches on the OpenCapSense boards that can switch the sensor
 * output from ECAP pins to ADC pins. The switching enables to switch two channels
 * at a time. It is not possible to switch one channel independently from the other one.
 *
 **/

#include "Device/Switch.h"

/**
 * initializes the adc/ecap SPDT switch
 * @return DEVICE_SWITCH_SUCCESS on success
 */
int Device_Switch_Init() {
	Output_Pins_Init(20);
	Output_Pins_Set(20, 1);
	Output_Pins_Init(21);
	Output_Pins_Set(21, 1);
	Output_Pins_Init(17);
	Output_Pins_Set(17, 1);
	Output_Pins_Init(18);
	Output_Pins_Set(18, 1);

	return DEVICE_SWITCH_SUCCESS;
}

/**
 * switch channel 1/2
 * @param high - switches to ECAP pins when 1 is given, to ADC pins when 0 is given
 * @return success
 */
int Device_Switch_Channel12(int high) {
	if(high == 1 || high == 0) {
		Output_Pins_Set(20, high);
		return DEVICE_SWITCH_SUCCESS;
	} else {
		return DEVICE_SWITCH_INVALIDPARAMETER;
	}
}


/**
 * switch channel 3/4
 * @param high - switches to ECAP pins when 1 is given, to ADC pins when 0 is given
 * @return success
 */
int Device_Switch_Channel34(int high) {
	if(high == 1 || high == 0) {
		Output_Pins_Set(21, high);
		return DEVICE_SWITCH_SUCCESS;
	} else {
		return DEVICE_SWITCH_INVALIDPARAMETER;
	}
}


/**
 * switch channel 5/6
 * @param high - switches to ECAP pins when 1 is given, to ADC pins when 0 is given
 * @return success
 */
int Device_Switch_Channel56(int high) {
	if(high == 1 || high == 0) {
		Output_Pins_Set(17, high);
		return DEVICE_SWITCH_SUCCESS;
	} else {
		return DEVICE_SWITCH_INVALIDPARAMETER;
	}
}

/**
 * switch channel 7/8
 * @param high - switches to ECAP pins when 1 is given, to ADC pins when 0 is given
 * @return success
 */
int Device_Switch_Channel78(int high) {
	if(high == 1 || high == 0) {
		Output_Pins_Set(18, high);
		return DEVICE_SWITCH_SUCCESS;
	} else {
		return DEVICE_SWITCH_INVALIDPARAMETER;
	}
}


/**
 * switch channel 7/8
 * @param high - switches to ECAP pins when 1 is given, to ADC pins when 0 is given
 * @return success
 */
int Device_Switch_AllChannels(int high) {
	if(high == 1 || high == 0) {
		Device_Switch_Channel12(high);
		Device_Switch_Channel34(high);
		Device_Switch_Channel56(high);
		Device_Switch_Channel78(high);

		return DEVICE_SWITCH_SUCCESS;
	} else {
		return DEVICE_SWITCH_INVALIDPARAMETER;
	}
}

//EOF
