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
 * @file SCI.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 * @brief Module for serial communication interface, aka UART.  
 *
 **/

#include "Com/SCI.h"

char Com_SCI_request[30];

/**
 * initializes sci gpio pins (26/27)
 * for piccolo control stick, the output is routed 
 * to xds 100 port b com port emulator
 **/
void Com_SCI_Init() {
	InitSciaGpio();
	
	EALLOW;
	
 	SciaRegs.SCICCR.all = 0x0007;   // 1 stop bit,  No loopback
                                   // No parity,8 char bits,
                                   // async mode, idle-line protocol
	SciaRegs.SCICTL1.all = 0x0003;  // enable TX, RX, internal SCICLK,
                                   // Disable RX ERR, SLEEP, TXWAKE

	SciaRegs.SCICTL2.bit.TXINTENA = 1;
	SciaRegs.SCICTL2.bit.RXBKINTENA = 1;

	SciaRegs.SCIHBAUD = 0x0000;
	SciaRegs.SCILBAUD = COM_SCI_BRR;

	// intializes sci fifo queue	
	SciaRegs.SCIFFTX.all=0xE040;
	SciaRegs.SCIFFRX.all=0x2044;
	SciaRegs.SCIFFCT.all=0x0;
	
	SciaRegs.SCICTL1.all = 0x0023;  // Relinquish SCI from Reset

	EDIS;
}

/**
 * Transmit a character from the SCI
 * @param int a
 **/
#pragma CODE_SECTION(Com_SCI_WaitForClearedBuffer, "ramfuncs");
void Com_SCI_WaitForClearedBuffer() {
    while(SciaRegs.SCIFFTX.bit.TXFFST > 0) {   }
}

/**
 * Transmit a character from the SCI
 * @param int a
 **/
#pragma CODE_SECTION(Com_SCI_Transmit, "ramfuncs");
void Com_SCI_Transmit(int a) {
	Com_SCI_WaitForClearedBuffer();
    SciaRegs.SCITXBUF = a;
}

/**
 * transmits a whole message which must be terminated by \0
 * @param char pointer to message
 **/
void Com_SCI_TransmitMessage(char* msg) {
	int i = 0;
	while (msg[i] != '\0') {
		Com_SCI_Transmit(msg[i]);
		i++;
	}
	Com_SCI_WaitForClearedBuffer();
}

/**
 * converts an int into a char array
 * @param the int given
 * @param the char array to copy the result into
 **/
void Com_SCI_IntToChar(long n, char s[]) {
     int i, j;
     long first;
     long last;
     long temp;
     int digits = 0;
     i = 0;

     if(n > 99999999) {
    	 s[0] = '-';
    	 s[1] = '1';
    	 s[2] = '\0';
    	 return;
     }
 
     if (n < 0) { /* record sign */
         n = -n;          /* make n positive */
         s[i] = 0;
         i++;
     }
     
     // determine number of digits
     first = 1;
     while(n >= (first * 10)) {
    	 first = first * 10;
    	 digits++;
     }

     digits++;
     last = first;
     temp = n;
     // now, go through the digits
     for(j = 0; j < digits; j++) {
    	 int result;

    	 result = floor(temp/last);
    	 temp = temp - (result * last);
    	 last = last / 10;

    	 s[i] = result + 48;
    	 i++;
     }
     
     s[i] = '\0';
}

/**
 * transmits an integer to SCI
 * @param positive int, maximum length of 5 digits
 * @return success
 */
int Com_SCI_TransmitLong(long n) {
	char buffer[8];
	if(n >= 0 && n < 1000000) {
		Com_SCI_IntToChar(n, buffer);
		Com_SCI_TransmitMessage(buffer);
		return COM_SCI_SUCCESS;
	} else {
		return COM_SCI_ERROR;
	}
}
 
 /**
  * reverses a character array
  * thanks to wikipedia for the sources.
  * @param string s
  **/
 void Com_SCI_CharReverse(char* s) {
     int i, j;
     char c;
 
     for (i = 0, j = Com_SCI_Strlen(s) - 1; i < j; i++, j--) {
         c = s[i];
         s[i] = s[j];
         s[j] = c;
     }
}

/**
 * checks wether a new request is there
 * @return zero if no request is there, 1 if new request is in Com_SCI_request
 **/
Uint16 Com_SCI_CheckRequest() {
	Uint16 i = 0;
	char last = 0; 
	
	if(SciaRegs.SCIFFRX.bit.RXFFST == 0) { 
		return 0;
	} 
	
	while(SciaRegs.SCIFFRX.bit.RXFFST > 0 
			&& (i < COM_SCI_REQUESTBUFFERSIZE)
			&& last != 4) { // last is not end of transmission
    	Com_SCI_request[i] = SciaRegs.SCIRXBUF.all;
    	last = Com_SCI_request[i];
    	i++;
	}
	
	return i;
}

 /**
  * counts the number of characters in string
  * thanks to wikipedia for the sources.
  * @param string s
  **/
Uint16 Com_SCI_Strlen(const char * str) {
    const char *s;
    for (s = str; *s; ++s);
    return(s - str);
}

/**
 * returns the request
 **/
char* Com_SCI_GetRequest() {
	return Com_SCI_request;
}

/**
 *sends the firmware information to sci port
 */
void Com_SCI_TransmitFirmwareInformation() {
	Com_SCI_TransmitMessage("Fraunhofer IGD\r\n");
	Com_SCI_TransmitMessage("Tobias Grosse-Puppendahl ");
	Com_SCI_TransmitMessage("<tobias.grosse-puppendahl@");
	Com_SCI_TransmitMessage("igd.fraunhofer.de>\r\n");
	Com_SCI_TransmitMessage("PiccoloBase Firmware");
	Com_SCI_TransmitMessage("\r\n");
	Com_SCI_TransmitMessage("=====================\r\n");
}

/**
 * transmits a value message over sci
 * @param the boardid
 * @param the sensor id
 * @param the value
 */
void Com_SCI_TransmitValueMessage(int boardId, int sensorId, long value) {
	Com_SCI_TransmitMessage("V,");
	Com_SCI_TransmitLong(boardId);
	Com_SCI_TransmitMessage(",");
	Com_SCI_TransmitLong(sensorId);
	Com_SCI_TransmitMessage(",");
	Com_SCI_TransmitLong(value);
	Com_SCI_TransmitMessage("\r\n");
}

// EOF
