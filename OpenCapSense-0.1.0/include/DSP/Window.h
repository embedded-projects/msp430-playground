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
 * @file Window.h
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#ifndef DSP_WINDOW_H_
#define DSP_WINDOW_H_

#include "F2806x_Device.h"

#include <math.h>

#define PI 3.14159265

void DSP_Window_Boxcar(int n, float32* w);
void DSP_Window_HanningRollOff(int n, float* w, int a);
void DSP_Window_Triangle(int n, float32* w);
void DSP_Window_Hanning(int n, float32* w);
void DSP_Window_Hamming(int n,float32* w);
void DSP_Window_Blackman(int n,float32* w);
void DSP_Window_Flattop(int n,float32* w);
float32 DSP_Window_Besselizero(float32 x);
void DSP_Window_Kaiser(int n, float32* w, float32 b);


#endif /*DSP_WINDOW_H_*/
