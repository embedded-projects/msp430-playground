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
 * @file Receiver.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief A receiver for shunt mode sensors using ADC functionality.
 * This module is still under construction!
 *
 **/

#include "Sensor/Receiver.h"

int Sensor_Receiver_gpios[8] = {0, 1, 2, 3, 4, 8, 6, 10};

/* 		ADC0 = Pin19 (A) / Pin22 (B)
 * 		ADC1 = Pin18 (A) / Pin23 (B)
 * 		ADC2 = Pin17 (A) / Pin24 (B)
 * 		ADC4 = Pin16 (A) / Pin25 (B)
 */
int Sensor_Receiver_sensorToADC[8] = {0, 1, 2, 4, 0, 1, 2, 4};

void (*Sensor_Receiver_callbacks[8])(float*, int) = {0, 0, 0, 0, 0, 0, 0, 0};

Utilities_SimpleBuffer Sensor_Receiver_buffers[2];
float Sensor_Receiver_values[2][SENSOR_RECEIVER_BUFFERSIZE];

/**
 * initializes a Sensor
 * @param sensorNumber the number of the sensor (between 1 and 8)
 * @param callback the function which is periodically called with the average data
 * @param (optional) the function which folds the data. if no function is given, the average value is calculated
 * @return returns an error code or 0 on success
 */
int Sensor_Receiver_Init(int sensorNumber, void (*callback)(float, int), float (*filter)(float*, int)) {

	// TODO

	return SENSOR_RECEIVER_SUCCESS;
}

/**
 * returns the physical number of an eCap
 * @param sensorNumber the number of the sensor (between 0 and 7)
 * @return the physical eCap number (between 0 and 2) or -1 on error
 */
int Sensor_Receiver_Sensor_To_ADC(int sensorNumber) {
	if(sensorNumber >= 0 && sensorNumber < 8) {
		return Sensor_Receiver_sensorToADC[sensorNumber];
	}

	// TODO

	return -1;
}

/**
 * starts the sensor and deactivates all other sensors on the same eCap
 * @param sensorNumber the number of the sensor (between 1 and 8)
 * @return returns an error code or 0 on success
 */
int Sensor_Receiver_Start(int sensorNumber) {
	int eCapNumber;
	if(sensorNumber < 0 || sensorNumber > 7) {
		return SENSOR_RECEIVER_INVALID_SENSORNUMBER;
	}
	if(!Sensor_Receiver_callbacks[sensorNumber]) {
		return SENSOR_RECEIVER_ILLEGAL_SENSORNUMBER;
	}

	// TODO

	return SENSOR_RECEIVER_SUCCESS;
}

/**
 * stops the sensor.
 * @param sensorNumber the number of the sensor (between 1 and 8)
 * @return returns an error code or 0 on success
 */
int Sensor_Receiver_Stop(int sensorNumber) {
	if(sensorNumber < 0 || sensorNumber > 7) {
		return SENSOR_RECEIVER_INVALID_SENSORNUMBER;
	}
	if(!Sensor_Receiver_callbacks[sensorNumber]) {
		return SENSOR_RECEIVER_ILLEGAL_SENSORNUMBER;
	}

	// TODO


	return SENSOR_RECEIVER_SUCCESS;
}

//EOF
