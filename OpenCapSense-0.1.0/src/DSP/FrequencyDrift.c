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
 * @file FrequencyDrift.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief This module is intended to compensate the drift in frequency with a few easy features.
 *
 **/
 
#include "DSP/FrequencyDrift.h"

int DSP_FrequencyDrift_LastMaximum = 0;

/**
 * sums up the bins of an array around the maximum.
 * @param the start bin to look from
 * @param the stop bin 
 * @param the range of bins to one side for summing up
 * @param the actual array
 * @return the sum around the maximum in range
 */
float DSP_FrequencyDrift_SumMaximum(int rangeStart, int rangeStop, int range, int thresholdFactor, float* target) {
	int i;
	int maximum = rangeStart;
	float result = 0;
	float threshold;
	
	// search for the maximum
	for(i = rangeStart + 1; i <= rangeStop; i++) {
		if(target[i] > target[maximum]) {
			maximum = i;
		}
	}
	
	DSP_FrequencyDrift_LastMaximum = maximum;
	threshold = target[maximum / thresholdFactor];
	
	// now sum up the fields arround the maximum
	for(i = maximum - range; i <= maximum + range; i++) {
		if(target[i] > threshold) {
			result += target[i];
		}
	} 
	
	return result;
}

// EOF
