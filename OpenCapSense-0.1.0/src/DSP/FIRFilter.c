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
 * @file FIRFilter.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief This module provides functionality to implement a FIR (finite impulse response) filter.
 *
 **/
 
#include "DSP/FIRFilter.h"

#include "DSP/FIRFilter_lp_fc_inf.h"
#include "DSP/FIRFilter_lp_fc_025.h"
#include "DSP/FIRFilter_lp_fc_05.h"
#include "DSP/FIRFilter_lp_fc_1.h"
#include "DSP/FIRFilter_lp_fc_2.h"
#include "DSP/FIRFilter_lp_fc_3.h"
#include "DSP/FIRFilter_lp_fc_4.h"
#include "DSP/FIRFilter_lp_fc_5.h"
#include "DSP/FIRFilter_lp_fc_6.h"
#include "DSP/FIRFilter_lp_fc_7.h"
#include "DSP/FIRFilter_lp_fc_8.h"
#include "DSP/FIRFilter_lp_test.h"

#include "DSP/FIRFilter_av_2.h"
#include "DSP/FIRFilter_av_3.h"
#include "DSP/FIRFilter_av_4.h"

float DSP_FIRFilter_history[DSP_FIRFILTER_NUMSENSORS][DSP_FIRFILTER_HISTORYSIZE]; /**< the history of measurements for each sensor id */

Uint16 DSP_FIRFilter_index[DSP_FIRFILTER_NUMSENSORS]; /**< Ring buffer index */

const float32* DSP_FIRFilter_kernels[DSP_FIRFILTER_NUMKERNELS] = {DSP_FIRFilter_lp_test, 
																	DSP_FIRFilter_lp_fc_2, 
																	DSP_FIRFilter_lp_fc_3, 
																	DSP_FIRFilter_lp_fc_4, 
																	DSP_FIRFilter_lp_fc_5, 
																	DSP_FIRFilter_lp_fc_6, 
																	DSP_FIRFilter_lp_fc_7, 
																	DSP_FIRFilter_lp_fc_8,
																	DSP_FIRFilter_lp_fc_1,
																	DSP_FIRFilter_lp_fc_05,
																	DSP_FIRFilter_lp_fc_025,
																	DSP_FIRFilter_lp_fc_inf,
																	DSP_FIRFilter_av_2,
																	DSP_FIRFilter_av_3,
																	DSP_FIRFilter_av_4}; /**< the kernel coefficients */

const int* DSP_FIRFilter_kernelSize[DSP_FIRFILTER_NUMKERNELS] = {&DSP_FIRFilter_lp_test_size,
																	&DSP_FIRFilter_lp_fc_2_size, 
																	&DSP_FIRFilter_lp_fc_3_size, 
																	&DSP_FIRFilter_lp_fc_4_size, 
																	&DSP_FIRFilter_lp_fc_5_size, 
																	&DSP_FIRFilter_lp_fc_6_size, 
																	&DSP_FIRFilter_lp_fc_7_size, 
																	&DSP_FIRFilter_lp_fc_8_size,
																	&DSP_FIRFilter_lp_fc_1_size,
																	&DSP_FIRFilter_lp_fc_05_size,
																	&DSP_FIRFilter_lp_fc_025_size,
																	&DSP_FIRFilter_lp_fc_inf_size,
																	&DSP_FIRFilter_av_2_size,
																	&DSP_FIRFilter_av_3_size,
																	&DSP_FIRFilter_av_4_size}; /**< the kernel sizes */

/**
 * initializes sensor calibration by setting the upper 
 **/
void DSP_FIRFilter_Init() {
	Uint16 i, j;
	
	for(i = 0; i < DSP_FIRFILTER_NUMSENSORS; i++) {
		for(j = 0; j < DSP_FIRFILTER_HISTORYSIZE; j++) {
			DSP_FIRFilter_history[i][j] = 0.0;
		}
	}
}

/**
 * filters a value with a given kernel from a given sensor
 * inserts the given value in the sensor's history 
 * @param the sensor id
 * @param the kernel index
 * @param the value
 * @return the filtered value
 **/
float32 DSP_FIRFilter_Filter(Uint16 sensorId, Uint16 kernelId, float32 value) {
	int kernelSize;
	Uint16 historyIndex;
	Uint16 i;
	int valueIndex;
	Uint16 kernelIndex;
	float32 result;
	
	historyIndex = DSP_FIRFilter_index[sensorId];
	kernelSize = *DSP_FIRFilter_kernelSize[kernelId];
	
	// increment history index in ring buffer
	if((++historyIndex) >= DSP_FIRFILTER_HISTORYSIZE) {
		historyIndex = 0;	
	}
	
	DSP_FIRFilter_history[sensorId][historyIndex] = value;
	DSP_FIRFilter_index[sensorId] = historyIndex;
	
	// now do the convolution with past values
	valueIndex = historyIndex;
	kernelIndex = kernelSize - 1;
	result = 0.0;
	for(i = 0; i < kernelSize; i++) {
		result += DSP_FIRFilter_kernels[kernelId][kernelIndex] * DSP_FIRFilter_history[sensorId][valueIndex];
		
		kernelIndex--;
		valueIndex--;
		
		if(valueIndex < 0) {
			valueIndex = (DSP_FIRFILTER_HISTORYSIZE - 1);
		}
	}
	
	return result;
}

// EOF
