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
 * @file FIRFilter.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef DSP_FIRFILTER_H_
#define DSP_FIRFILTER_H_

#include "F2806x_Device.h"

#define DSP_FIRFILTER_NUMSENSORS 16
#define DSP_FIRFILTER_HISTORYSIZE 16
#define DSP_FIRFILTER_NUMKERNELS 15

// these are the possible kernel ids
#define DSP_FIRFILTER_KERNELTEST 0 // test kernel, not for productive usage

#define DSP_FIRFILTER_KERNELFC2 1 // cutoff frequency 2 Hz
#define DSP_FIRFILTER_KERNELFC3 2 // cutoff frequency 3 Hz
#define DSP_FIRFILTER_KERNELFC4 3 // cutoff frequency 4 Hz
#define DSP_FIRFILTER_KERNELFC5 4 // cutoff frequency 5 Hz
#define DSP_FIRFILTER_KERNELFC6 5 // cutoff frequency 6 Hz
#define DSP_FIRFILTER_KERNELFC7 6 // cutoff frequency 7 Hz
#define DSP_FIRFILTER_KERNELFC8 7 // cutoff frequency 8 Hz

#define DSP_FIRFILTER_KERNELFC1 8 // cutoff frequency 1 Hz
#define DSP_FIRFILTER_KERNELFC05 9 // cutoff frequency 0.5 Hz
#define DSP_FIRFILTER_KERNELFC025 10 // cutoff frequency 0.25 Hz

#define DSP_FIRFILTER_KERNELFCINF 11 // cutoff frequency 0.25 Hz

#define DSP_FIRFILTER_KERNELAV2 12 // average of 2
#define DSP_FIRFILTER_KERNELAV3 13 // average of 3
#define DSP_FIRFILTER_KERNELAV4 14 // average of 3

void DSP_FIRFilter_Init();
float32 DSP_FIRFilter_Filter(Uint16 sensorId, Uint16 kernelId, float32 value);

#endif /*DSP_FIRFILTER_H_*/

// EOF
