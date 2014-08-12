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
 * @file AstableMultivibrator.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief This module capsulates functionality for reading an astable multivibrator sensor.
 * It uses the Input/ECAP module to capture the edges of the sensor signal.
 *
 **/

#include "Sensor/AstableMultivibrator.h"

int Sensor_AstableMultivibrator_gpios[8] = {0, 1, 2, 3, 4, 8, 6, 10};

#if DEVICE_INIT_REVISION > 2
int Sensor_AstableMultivibrator_ecapGPIOs[8] = {5, 24, 15, 26, 11, 25, 7, 9};
#else
int Sensor_AstableMultivibrator_ecapGPIOs[8] = {5, 24, 15, 26, 11, 19, 7, 9};
#endif

#if DEVICE_INIT_REVISION > 2
int Sensor_AstableMultivibrator_sensorToECap[8] = {0, 0, 1, 2, 0, 1, 1, 2};
#else
int Sensor_AstableMultivibrator_sensorToECap[8] = {0, 0, 1, 2, 0, 0, 1, 2};
#endif

/**
 * initializes a Sensor
 * @param sensorNumber the number of the sensor (between 1 and 8)
 * @return returns an error code or 0 on success
 */
int Sensor_AstableMultivibrator_Init(int sensorNumber) {
	if(sensorNumber < 0 || sensorNumber > 7) {
		return SENSOR_ASTABLEMULTIVIBRATOR_INVALID_SENSORNUMBER;
	}
	if(Output_Pins_Init(Sensor_AstableMultivibrator_gpios[sensorNumber])) {
		return SENSOR_ASTABLEMULTIVIBRATOR_PIN_NOT_INITIALIZED;
	}

	Input_ECAP_Init(Sensor_AstableMultivibrator_ecapGPIOs[sensorNumber]);
	Output_Pins_Set(Sensor_AstableMultivibrator_gpios[sensorNumber], 0);

	return SENSOR_ASTABLEMULTIVIBRATOR_SUCCESS;
}

/**
 * sets the dismiss cycles for measuring
 */
void Sensor_AstableMultivibrator_SetDismissCycles(int number) {
	if(number < 0 || number > 1000) {
		return;
	}

	Input_ECAP_SetDismissCycles(number);
}

/**
 * returns the physical number of an eCap
 * @param sensorNumber the number of the sensor (between 0 and 7)
 * @return the physical eCap number (between 0 and 2) or -1 on error
 */
int Sensor_AstableMultivibrator_Sensor_To_eCap(int sensorNumber) {
	if(sensorNumber < 0 || sensorNumber > 7) {
		return SENSOR_ASTABLEMULTIVIBRATOR_INVALID_SENSORNUMBER;
	}

	return Sensor_AstableMultivibrator_sensorToECap[sensorNumber];
}

/**
 * starts the sensor and deactivates all other sensors on the same eCap
 * @param sensorNumber the number of the sensor (between 1 and 8)
 * @return returns an error code or 0 on success
 */
int Sensor_AstableMultivibrator_Start(int sensorNumber) {
	int eCapNumber;
	int enableGpio;

	if(sensorNumber < 0 || sensorNumber > 7) {
		return SENSOR_ASTABLEMULTIVIBRATOR_INVALID_SENSORNUMBER;
	}

	eCapNumber = Sensor_AstableMultivibrator_Sensor_To_eCap(sensorNumber);
	enableGpio = Sensor_AstableMultivibrator_gpios[sensorNumber];
	Output_Pins_Set(enableGpio, 1);

	// TODO: This should be compatible with the old version V2
	if(eCapNumber == 0) {
		Output_Pins_Set_IfNotPin(Sensor_AstableMultivibrator_gpios[0], enableGpio, 0);
		Output_Pins_Set_IfNotPin(Sensor_AstableMultivibrator_gpios[1], enableGpio, 0);
		Output_Pins_Set_IfNotPin(Sensor_AstableMultivibrator_gpios[4], enableGpio, 0);
	}
	else if(eCapNumber == 1) {
		Output_Pins_Set_IfNotPin(Sensor_AstableMultivibrator_gpios[2], enableGpio, 0);
		Output_Pins_Set_IfNotPin(Sensor_AstableMultivibrator_gpios[6], enableGpio, 0);
		Output_Pins_Set_IfNotPin(Sensor_AstableMultivibrator_gpios[5], enableGpio, 0);
	}
	else {
		Output_Pins_Set_IfNotPin(Sensor_AstableMultivibrator_gpios[3], enableGpio, 0);
		Output_Pins_Set_IfNotPin(Sensor_AstableMultivibrator_gpios[7], enableGpio, 0);
	}

	Input_ECAP_Enable(Sensor_AstableMultivibrator_ecapGPIOs[sensorNumber]);

	return SENSOR_ASTABLEMULTIVIBRATOR_SUCCESS;
}

/**
 * stops the sensor.
 * @param sensorNumber the number of the sensor (between 1 and 8)
 * @return returns an error code or 0 on success
 */
int Sensor_AstableMultivibrator_Stop(int sensorNumber) {
	if(sensorNumber < 0 || sensorNumber > 7) {
		return SENSOR_ASTABLEMULTIVIBRATOR_INVALID_SENSORNUMBER;
	}

	Input_ECAP_Disable(Sensor_AstableMultivibrator_ecapGPIOs[sensorNumber]);
	Output_Pins_Set(Sensor_AstableMultivibrator_gpios[sensorNumber], 0);

	DELAY_US(10);

	return SENSOR_ASTABLEMULTIVIBRATOR_SUCCESS;
}

/**
 * returns the result measured from ecaps
 * @param the sensorNumber
 * @return result
 */
float Sensor_AstableMultivibrator_GetResult(int sensorNumber) {
	if(sensorNumber < 0 || sensorNumber > 7) {
		return SENSOR_ASTABLEMULTIVIBRATOR_INVALID_SENSORNUMBER;
	}

	return Input_ECAP_GetResult(sensorNumber);
}

/**
 * returns the number of results measured from ecaps
 * @param the sensorNumber
 * @return result
 */
int Sensor_AstableMultivibrator_GetValueCount(int sensorNumber) {
	if(sensorNumber < 0 || sensorNumber > 7) {
		return SENSOR_ASTABLEMULTIVIBRATOR_INVALID_SENSORNUMBER;
	}

	return Input_ECAP_GetValueCountWithoutDismissCycles(sensorNumber);
}

/**
 * returns the average result from ecaps
 * @param the sensorNumber
 * @return result
 */
float Sensor_AstableMultivibrator_GetAverageResult(int sensorNumber) {
	if(sensorNumber < 0 || sensorNumber > 7) {
		return SENSOR_ASTABLEMULTIVIBRATOR_INVALID_SENSORNUMBER;
	}
	if(Input_ECAP_GetValueCountWithoutDismissCycles(sensorNumber) <= 0) {
		return 0;
	}

	float result = (Input_ECAP_GetResult(sensorNumber)) / ((float) Input_ECAP_GetValueCountWithoutDismissCycles(sensorNumber));

	return result;
}

//EOF
