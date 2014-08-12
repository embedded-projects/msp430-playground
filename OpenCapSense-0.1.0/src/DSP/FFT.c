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
 * @brief Module for frequency division multiple access, uses fft for demodulation.
 * this module primarily demodulates the input data, no real modulation is realized in software
 *
 **/

#include "DSP/FFT.h"

#pragma DATA_SECTION(DSP_FFT_Out, "DSP_FFT_Out");         //If using RFFT_f32u, this

float DSP_FFT_Out[DSP_FFT_SIZE];
float DSP_FFT_CosSinTable[DSP_FFT_SIZE];
float DSP_FFT_Magnitude[DSP_FFT_SIZE/2+1];
RFFT_F32_STRUCT DSP_FFT_Rfft;

/**
 * intializes fdma
 **/
void DSP_FFT_Init() {
    DSP_FFT_Rfft.FFTSize  = DSP_FFT_SIZE;
    DSP_FFT_Rfft.FFTStages = DSP_FFT_STAGES;
    DSP_FFT_Rfft.OutBuf = &DSP_FFT_Out[0];             //Output buffer
    DSP_FFT_Rfft.InBuf = 0;             // In buffer
    DSP_FFT_Rfft.CosSinBuf = &DSP_FFT_CosSinTable[0];  //Twiddle factor buffer
    DSP_FFT_Rfft.MagBuf = &DSP_FFT_Magnitude[0];	   //Magnitude buffer
}

/**
 * decodes a sampling buffer from time to frequency domain
 *
 * @param the input buffer, needs buffer alignment using pragma data section with alignment (! IMPORTANT !)
 * @param the target, just a pointer which is set on RFFT result
 **/
float* DSP_FFT_Decode(float* buffer) {
    int i;

    DSP_FFT_Rfft.InBuf = &buffer[0];

	RFFT_f32_sincostable(&DSP_FFT_Rfft); //Calculate twiddle factor
   	RFFT_f32u(&DSP_FFT_Rfft);	//Calculate real FFT
    RFFT_f32_mag(&DSP_FFT_Rfft); //Calculate magnitude

    // we do not want to have the small frequencies
    // this is done for better debugging with code composer graphs :-)
    for(i = 0; i < 25; i++) {
    	DSP_FFT_Magnitude[i] = 0.0;
    }

    return &DSP_FFT_Magnitude[0];
}

/**
 * cleans the fft output arrays
 **/
void DSP_FFT_CleanFFT() {
	int i;

	for (i = 0; i < DSP_FFT_SIZE; i++) {
      	DSP_FFT_Out[i] = 0;
    }

    for (i = 0; i < DSP_FFT_SIZE/2; i++) {
     	DSP_FFT_Magnitude[i] = 0;
    }
}

// EOF
