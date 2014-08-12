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
 * @file I2C.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#include "Com/I2C.h"

void (*Com_I2C_isrwrite)(int, int) = 0;
void (*Com_I2C_isrread)(int, int, int, int*) = 0;

int Com_I2C_block = -1; /**< Used for synchronous  read/write. Blocks the function until the isr is reached */
int Com_I2C_address = 0; /**< holds the slave-address of the device */
int Com_I2C_bytesToRead = 0; /**< number of bytes to be read */
int *Com_I2C_readBuffer = 0; /**< pointer to an int-buffer. The received data will be written to it */

/**
 * initializes the I2C bus
 * @return 0 on success
 */
int Com_I2C_Init() {

	IER |= M_INT8;

	EALLOW;
		GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO28 (SDAA)
		GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;    // Enable pull-up for GPIO29 (SCLA)

		GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // Asynch input GPIO28 (SDAA)
		GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // Asynch input GPIO29 (SCLA)

		GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;   // Configure GPIO28 for SDAA operation
		GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 1;   // Configure GPIO29 for SCLA operation
	EDIS;

	// Initialize I2C
	I2caRegs.I2CMDR.bit.IRS = 0;	//disable I2C

	//I2caRegs.I2CPSC.all = 7;		//prescaler = 10MHz
	I2caRegs.I2CPSC.all = 7;		//prescaler = 10MHz
	//I2caRegs.I2CCLKL = 45;			//low = 5us
	//I2caRegs.I2CCLKH = 45;			//high = 5us
	I2caRegs.I2CCLKL = 7;			//low = 1.25us
	I2caRegs.I2CCLKH = 7;			//high = 1.25us
	I2caRegs.I2CIER.bit.SCD = 1;	//stop-condition interrupt
	//I2caRegs.I2CIER.bit.NACK = 1;	//NACK interrupt

	I2caRegs.I2CMDR.bit.XA = 0;		//7bit adress
	I2caRegs.I2CMDR.bit.FDF = 0;	//no freedata-format
	I2caRegs.I2CMDR.bit.FREE = 0;	//Stops if debugger stops
	I2caRegs.I2CMDR.bit.MST = 1;	//enable master-mode

	I2caRegs.I2CFFTX.all = 0x6000;	// Enable FIFO mode and TXFIFO
	I2caRegs.I2CFFRX.all = 0x2040;	// Enable RXFIFO, clear RXFFINT,
	I2caRegs.I2CMDR.bit.IRS = 1;	//enable I2C

	PieCtrlRegs.PIEIER8.bit.INTx1 = 1;
	EALLOW;
		PieVectTable.I2CINT1A = &Com_I2C_ISR;
	EDIS;

	return COM_I2C_SUCCESS;
}

/**
 * sends asynchronously data over the I2C bus. When the transmission of data is complete or failed, the callbackfunction is called
 * @param address the slave address of the connected device
 * @param numberOfBytes the number of bytes to be sent
 * @param data an integer array of the data to be sent
 * @param callback the interrupt service routine which is called after the transmission has completed or failed. The first parameter of the isr equals 0 if the
 * 		transmission has failed. The second parameter holds the slave address.
 * @return an error code or 0 if successful
 */
int Com_I2C_WriteData_Async(int address, int numberOfBytes, int *data, void (*callback)(int, int)) {
	int i = 0;
	if(!numberOfBytes) {
		return COM_I2C_NO_DATA;
	}
	if(!callback) {
		return COM_I2C_INVALID_CALLBACK;
	}
	if(I2caRegs.I2CMDR.bit.STP == 1) {
	   return COM_I2C_NOT_READY;
	}

	Com_I2C_isrwrite = callback;
	Com_I2C_isrread = 0;
	Com_I2C_address = address;
	I2caRegs.I2CSAR = address;	//setup slave address

	if (I2caRegs.I2CSTR.bit.BB == 1) {
	   return COM_I2C_BUS_BUSY;
	}

	I2caRegs.I2CCNT = numberOfBytes;		//number of bytes to send

	for(i = 0; i < numberOfBytes; i++) {
		I2caRegs.I2CDXR = data[i];
	}

	I2caRegs.I2CMDR.all = 0x2E20;	//0010 1110 0010 0000	0x2E20 = write, 0x2C20 = read

	return COM_I2C_SUCCESS;
}

/**
 * sends synchronously data over the I2C bus. During the transmission the the current thread is blocked until the transmission ends.
 * @param address the slave address of the connected device
 * @param numberOfBytes the number of bytes to be sent
 * @param data an integer array of the data to be sent
 * @return an error code or 0 if successful
 */
int Com_I2C_WriteData_Sync(int adress, int numberOfBytes, int *data) {
	long i = 0;
	if(!numberOfBytes) {
		return COM_I2C_NO_DATA;
	}
	if(I2caRegs.I2CMDR.bit.STP == 1) {
	   return COM_I2C_NOT_READY;
	}

	Com_I2C_isrwrite = 0;
	Com_I2C_isrread = 0;
	I2caRegs.I2CSAR = adress;	//setup slave address

	if (I2caRegs.I2CSTR.bit.BB == 1) {
	   return COM_I2C_BUS_BUSY;
	}

	I2caRegs.I2CCNT = numberOfBytes;		//number of bytes to send

	for(i = 0; i < numberOfBytes; i++) {
		I2caRegs.I2CDXR = data[i];
	}

	I2caRegs.I2CMDR.all = 0x2E20;	//0010 1110 0010 0000	0x2E20 = write, 0x2C20 = read

	i = 0;
	Com_I2C_block = 0;
	while(!Com_I2C_block) {
		i++;
		if(i > COM_I2C_TIMEOUT) {
			return COM_I2C_UNKNOWN_ERROR;
		}
	}

	if(Com_I2C_block == 0x6) {
		return COM_I2C_SUCCESS;
	}
	else {
		return COM_I2C_SEND_FAIL;
	}
}


/**
 * reads asynchronously data over the I2C bus. When the receiving of data is complete or failed, the callbackfunction is called
 * @param address the slave address of the connected device
 * @param numberOfBytes the number of bytes to be read
 * @param readBuffer the array to which the data is written
 * @param callback the callback service routine which is called after the receiving has completed or failed. The first parameter of the isr equals 0 if the
 * 		transmission has failed. The second parameter holds the slave address. Third parameter is the array size and the fourth the array pointer
 * @return an error code or 0 if successful
 */
int Com_I2C_ReadData_Async(int address, int numberOfBytes, int *readBuffer, void (*callback)(int, int, int, int*)) {
	if(!numberOfBytes) {
		return COM_I2C_NO_DATA;
	}
	if(!callback) {
		return COM_I2C_INVALID_CALLBACK;
	}
	if(I2caRegs.I2CMDR.bit.STP == 1) {
	   return COM_I2C_NOT_READY;
	}
	if(!readBuffer) {
		return COM_I2C_INVALID_BUFFER;
	}
	Com_I2C_isrread = callback;
	Com_I2C_isrwrite = 0;
	Com_I2C_readBuffer = readBuffer;
	Com_I2C_address = address;
	Com_I2C_bytesToRead = numberOfBytes;

	I2caRegs.I2CSAR = address;	//setup slave address

	if (I2caRegs.I2CSTR.bit.BB == 1) {
	   return COM_I2C_BUS_BUSY;
	}

	I2caRegs.I2CCNT = numberOfBytes;		//number of bytes to send

	I2caRegs.I2CMDR.all = 0x2C20;	//0010 1110 0010 0000	0x2E20 = write, 0x2C20 = read
	return COM_I2C_SUCCESS;
}

/**
 * reads synchronously data over the I2C bus. During the receiving the current thread is blocked until the transmission ends.
 * @param address the slave address of the connected device
 * @param numberOfBytes the number of bytes to be read
 * @param readBuffer the array to which the data is written
 * @return an error code or 0 if successful
 */
int Com_I2C_ReadData_Sync(int address, int numberOfBytes, int *readBuffer) {
	long i = 0;
	if(!numberOfBytes) {
		return COM_I2C_NO_DATA;
	}
	if(I2caRegs.I2CMDR.bit.STP == 1) {
	   return COM_I2C_NOT_READY;
	}
	if(!readBuffer) {
		return COM_I2C_INVALID_BUFFER;
	}

	Com_I2C_isrwrite = 0;
	Com_I2C_isrread = 0;
	I2caRegs.I2CSAR = address;	//setup slave address

	if (I2caRegs.I2CSTR.bit.BB == 1) {
	   return COM_I2C_BUS_BUSY;
	}

	I2caRegs.I2CCNT = numberOfBytes;		//number of bytes to send

	I2caRegs.I2CMDR.all = 0x2C20;	//0010 1110 0010 0000	0x2E20 = write, 0x2C20 = read

	i = 0;
	Com_I2C_block = 0;
	while(!Com_I2C_block) {
		i++;
		if(i > COM_I2C_TIMEOUT) {
			return COM_I2C_UNKNOWN_ERROR;
		}
	}

	for(i = 0; i < Com_I2C_bytesToRead; i++) {
		Com_I2C_readBuffer[i] = I2caRegs.I2CDRR;
	}

	if(Com_I2C_block == 0x6) {
		return COM_I2C_SUCCESS;
	}
	else {
		return COM_I2C_READ_FAIL;
	}
}

/**
 * the isr of the I2C module (only used internally)
 */
interrupt void Com_I2C_ISR(void) {
	int i = 0;
	I2caRegs.I2CSTR.bit.NACKSNT = 1;
	Com_I2C_block = I2caRegs.I2CISRC.bit.INTCODE;
	if(Com_I2C_isrwrite) {
		Com_I2C_isrwrite(Com_I2C_block == 0x6, Com_I2C_address);
	}
	if(Com_I2C_isrread) {
		for(i = 0; i < Com_I2C_bytesToRead; i++) {
			Com_I2C_readBuffer[i] = I2caRegs.I2CDRR;
		}
		Com_I2C_isrread(Com_I2C_block == 0x6, Com_I2C_address, Com_I2C_bytesToRead, Com_I2C_readBuffer);
	}
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;
}

//EOF
