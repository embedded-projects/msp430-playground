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
 * @file FFT.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef DSP_FFT_H_
#define DSP_FFT_H_

#include "Utilities/SimpleBuffer.h"
#include "math.h"
#include "float.h"
#include "FPU.h"

#define	DSP_FFT_STAGES 8
#define	DSP_FFT_SIZE (1 << DSP_FFT_STAGES)

void DSP_FFT_Init();
float* DSP_FFT_Decode(float32* buffer);

#endif /*DSP_FFT_H_*/

// EOF
