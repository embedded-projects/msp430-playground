/**
 *
 * This file is part of OpenCapSense Firmware (www.opencapsense.org).
 *
 * This file was adopted from code written by Anders Johansson <ajh@atri.curtin.edu.au>
 * released under GPL.
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
 * @author Anders Johansson <ajh@atri.curtin.edu.au>
 * @brief Calculates a number of window functions. The following window
 * functions are currently implemented: Boxcar, Triang, Hanning,
 * Hamming, Blackman, Flattop and Kaiser. In the function call n is
 * the number of filter taps and w the buffer in which the filter
 * coefficients will be stored.
 *
 **/

#include "DSP/Window.h"

/**
 * Boxcar
 * @param n window length
 * @param w buffer for the window parameters
*/
void DSP_Window_Boxcar(int n, float* w) {
  	int i;
  	for (i=0 ; i<n ; i++)
    	w[i] = 1.0;
}


/**
 * Hanning Roll off window
 * @param n window length
 * @param w window
 * @param a roll off indexes at each side
*/
void DSP_Window_HanningRollOff(int n, float* w, int a) {
  	int i, j;
  	
  	DSP_Window_Hanning((a * 2), w);
  	
  	j = 0;
  	for (i = n - a; i < n ; i++) {
    	w[i] = w[j + a];
    	j++;
    }
  	
  	for(i = a; i < n - a; i++) {
    	w[i] = 1.0;
  	}
}


/**
 * Triang a.k.a Bartlett
 *               |    (N-1)| 
 *           2 * |k - -----|
 *               |      2  |
 * w = 1.0 - ---------------
 *                    N+1
 * @param n window length
 * @param w buffer for the window parameters
*/
void DSP_Window_Triangle(int n, float* w) {
  	float k1  = (float)(n & 1);
  	float k2  = 1/((float)n + k1);
  	int end = (n + 1) >> 1;
  	int	i;
  
  	for (i=0 ; i<end ; i++) {
    	w[i] = w[n-i-1] = (2.0*((float)(i+1))-(1.0-k1))*k2;
  	}
}


/**
 * Hanning
 *                   2*pi*k
 * w = 0.5 - 0.5*cos(------), where 0 < k <= N
 *                    N+1
 * @param n window length
 * @param w buffer for the window parameters
 **/
void DSP_Window_Hanning(int n, float32* w) {
  	int	   i;
  	float32 k = 2*PI/((float32)(n+1)); // * 2*pi/(N+1)
  
  	for (i=0; i<n; i++) {
    	*w++ = 0.5*(1.0 - cos(k*(float32)(i+1)));
  	}
}

/**
 * Hamming
 *                        2*pi*k
 * w(k) = 0.54 - 0.46*cos(------), where 0 <= k < N
 *                         N-1
 * @param n window length
 * @param w buffer for the window parameters
 **/
void DSP_Window_Hamming(int n,float32* w) {
  	int      i;
  	float32 k = 2*PI/((float32)(n-1));  //* 2*pi/(N-1)

  	for (i=0; i<n; i++) {
    	*w++ = 0.54 - 0.46*cos(k*(float32)i);
  	}
}

/**
 * Blackman
 *                       2*pi*k             4*pi*k
 * w(k) = 0.42 - 0.5*cos(------) + 0.08*cos(------), where 0 <= k < N
 *                        N-1                 N-1
 * @param n window length
 * @param w buffer for the window parameters
 **/
void DSP_Window_Blackman(int n,float32* w) {
  	int      i;
  	float32 k1 = 2*PI/((float32)(n-1)); // * 2*pi/(N-1)
  	float32 k2 = 2*k1;  // 4*pi/(N-1)

  	for (i=0; i<n; i++) {
    	*w++ = 0.42 - 0.50*cos(k1*(float32)i) + 0.08*cos(k2*(float32)i);
  	}
}

/**
 * Flattop
 *                                        2*pi*k                     4*pi*k
 * w(k) = 0.2810638602 - 0.5208971735*cos(------) + 0.1980389663*cos(------), where 0 <= k < N
 *                                          N-1                        N-1
 * @param n window length
 * @param w buffer for the window parameters
 **/
void DSP_Window_Flattop(int n,float32* w) {
  	int      i;
  	float32 k1 = 2*PI/((float32)(n-1)); // 2*pi/(N-1)
  	float32 k2 = 2*k1; // * 4*pi/(N-1)
  
  	for (i=0; i<n; i++) {
    	*w++ = 0.2810638602 - 0.5208971735*cos(k1*(float32)i) + 0.1980389663*cos(k2*(float32)i);
	}
}

/** 
 * Computes the 0th order modified Bessel function of the first kind.  
 * (Needed to compute Kaiser window) 
 *   
 * y = sum( (x/(2*n))^2 )
 * @param n
 * @return the bessel function result
 **/
#define BIZ_EPSILON 1E-21 // * Max error acceptable 

float32 DSP_Window_Besselizero(float32 x) { 
  	float32 temp;
  	float32 sum   = 1.0;
  	float32 u     = 1.0;
  	float32 halfx = x/2.0;
  	int      n     = 1;

  	do {
    	temp = halfx/(float32)n;
    	u *=temp * temp;
    	sum += u;
    	n++;
  	} while (u >= BIZ_EPSILON * sum);
  	
  	return(sum);
}

/**
 * Kaiser
 *
 * @paramn window length
 * @paramw buffer for the window parameters
 * @param b beta parameter of Kaiser window, Beta >= 1
 *
 * Beta trades the rejection of the low pass filter against the
 * transition width from passband to stop band.  Larger Beta means a
 * slower transition and greater stop band rejection.  See Rabiner and
 * Gold (Theory and Application of DSP) under Kaiser windows for more
 * about Beta.  The following table from Rabiner and Gold gives some
 * feel for the effect of Beta:
 * 
 * All ripples in dB, width of transition band = D*N where N = window
 * length
 * 
 * BETA    D       PB RIP   SB RIP
 * 2.120   1.50  +-0.27      -30
 * 3.384   2.23    0.0864    -40
 * 4.538   2.93    0.0274    -50
 * 5.658   3.62    0.00868   -60
 * 6.764   4.32    0.00275   -70
 * 7.865   5.0     0.000868  -80
 * 8.960   5.7     0.000275  -90
 * 10.056  6.4     0.000087  -100
 **/
void DSP_Window_Kaiser(int n, float32* w, float32 b) {
  	float32 tmp;
  	float32 k1  = 1.0/DSP_Window_Besselizero(b);
  	int	   k2  = 1 - (n & 1);
  	int      end = (n + 1) >> 1;
  	int      i; 
  
   	
  	for (i=0 ; i<end ; i++) {
    	tmp = (float32)(2*i + k2) / ((float32)n - 1.0);
    	w[end-(1&(!k2))+i] = w[end-1-i] = k1 * DSP_Window_Besselizero(b*sqrt(1.0 - tmp*tmp));
  	}
}

// EOF
