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
 * @file CAN.c
 * @author Tobias Grosse-Puppendahl <tobias.grosse-puppendahl@igd.fraunhofer.de>
 *
 **/

#include "Com/CAN.h"

long Com_CAN_BaseAddress = COM_CAN_BASEADDRESS;
long Com_CAN_BoardId = COM_CAN_BASEADDRESS;

Uint32 Com_CAN_messageObjectTimestamps[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

/**
 * initializes the I2C bus
 * @return 0 on success
 */
int Com_CAN_Init() {
	int i;

	InitECanGpio();

	InitECan();

    // Configure the eCAN for self test mode
    // Enable the enhanced features of the eCAN.
    //EALLOW;
    //ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    //ECanaShadow.CANMC.bit.STM = 1;    // Configure CAN for self-test mode
    //ECanaShadow.CANMC.bit. = 1;    // Configure CAN for self-test mode
    //ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    //EDIS;

    // reset all timestamps
    for(i = 0; i < 32; i++) {
    	volatile Uint32* mots;

    	mots = &ECanaMOTSRegs.MOTS0 + i;
    	*mots = 0;
    }

	Com_CAN_InitReceiveMBoxes();
	Com_CAN_InitTransmitMBoxes();

	return 0;
}

/**
 * create a transmission with MBOX n
 * @param the board id
 * @param the number of sensor
 * @param the measured value
 */
void Com_CAN_TransmitValueMessage(int sensor1, int val1, int sensor2, int val2) {
	struct ECAN_REGS ECanaShadow;

	ECanaMboxes.MBOX0.MDL.all = 0;
	ECanaMboxes.MBOX0.MDL.byte.BYTE0 = (Uint8) 86; // "V" for value
	ECanaMboxes.MBOX0.MDL.byte.BYTE1 = (Uint8) sensor1; // the number of sensor
	ECanaMboxes.MBOX0.MDL.word.LOW_WORD = (Uint16) val1;

	ECanaMboxes.MBOX0.MDH.all = 0;
	ECanaMboxes.MBOX0.MDH.byte.BYTE4 = (Uint8) 86; // "V" for value
	ECanaMboxes.MBOX0.MDH.byte.BYTE5 = (Uint8) sensor2; // the number of sensor
	ECanaMboxes.MBOX0.MDH.word.LOW_WORD = (Uint16) val2;

    ECanaShadow.CANTRS.all = 0;
	ECanaShadow.CANTRS.bit.TRS0 = 1;
	ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;

	// uncomment if you require an ACK
	do {
		ECanaShadow.CANTA.all = ECanaRegs.CANTA.all;
	} while(ECanaShadow.CANTA.bit.TA0 == 0); // Wait for TA5 bit to be set..

    ECanaShadow.CANTA.all = 0;
    ECanaShadow.CANTA.bit.TA0 = 1;	// Clear TA8
    ECanaRegs.CANTA.all = ECanaShadow.CANTA.all;
}

/**
 * create a transmission with MBOX n
 * @param the board id
 * @param the number of sensor
 * @param the measured value
 */
void Com_CAN_TransmitStartMeasurementMessage(int boardId) {
	struct ECAN_REGS ECanaShadow;

	ECanaMboxes.MBOX1.MDL.all = 0;
	ECanaMboxes.MBOX1.MDL.byte.BYTE0 = 0; // "V" for value
	ECanaMboxes.MBOX1.MDL.byte.BYTE1 = 0; // the number of sensor
	ECanaMboxes.MBOX1.MDL.word.LOW_WORD = (Uint16) boardId;

	ECanaMboxes.MBOX1.MDH.all = 0x55555555;

    ECanaShadow.CANTRS.all = 0;
	ECanaShadow.CANTRS.bit.TRS1 = 1;
	ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;

	// uncomment if you require an ACK
	do {
		ECanaShadow.CANTA.all = ECanaRegs.CANTA.all;
	} while(ECanaShadow.CANTA.bit.TA1 == 0); // Wait for TA5 bit to be set..

    ECanaShadow.CANTA.all = 0;
    ECanaShadow.CANTA.bit.TA1 = 1;	// Clear TA8
    ECanaRegs.CANTA.all = ECanaShadow.CANTA.all;
}

/**
 * returns the value message with extracted values
 */
void Com_CAN_ReadValueMessage(int mbox, char* messageId, int* boardId, int* sensor1, int* value1, int* sensor2, int* value2) {
	volatile struct MBOX *Mailbox;
	Mailbox = &ECanaMboxes.MBOX0 + mbox;

	*boardId = (int) mbox - 16;
	*messageId = (char) Mailbox->MDL.byte.BYTE0; // "V" for value
	*sensor1 = (int) Mailbox->MDL.byte.BYTE1; // the number of sensor
	*value1 = (int) Mailbox->MDL.word.LOW_WORD;
	*sensor2 = (int) Mailbox->MDH.byte.BYTE5; // the number of sensor
	*value2 = (int) Mailbox->MDH.word.LOW_WORD;

	return;
}

/**
 * returns the value message with extracted values
 */
void Com_CAN_ReadStartMeasurementMessage(int* boardId) {
	volatile struct MBOX *Mailbox;
	int mbox = 31;
	Mailbox = &ECanaMboxes.MBOX0 + mbox;

	*boardId = (int) Mailbox->MDL.word.LOW_WORD;

	return;
}

/**
 * checks if new messages are available on mbox 16-31, resets the message object time stamp (mots) when
 * returning the updated result
 * @return the number of mbox that is updated or -1 if no mailbox was updated
 */
int Com_CAN_CheckValueMessageBoxes() {
	int i;

    // check all timestamps
    for(i = 16; i < 31; i++) {
    	volatile Uint32* mots;

    	mots = &ECanaMOTSRegs.MOTS0 + i;
    	if(*mots != Com_CAN_messageObjectTimestamps[i-16]) {
    		Com_CAN_messageObjectTimestamps[i-16] = *mots;
    		return i;
    	}
    }

    return -1;
}

/**
 * checks if a new start measurement message is available
 * returning the updated result
 * @return the number of mbox that is updated or -1 if no mailbox was updated
 */
int Com_CAN_CheckStartMeasurementMessageBox() {
	int i = 31;

	volatile Uint32* mots;

	mots = &ECanaMOTSRegs.MOTS0 + i;
	if(*mots != Com_CAN_messageObjectTimestamps[i-16]) {
		Com_CAN_messageObjectTimestamps[i-16] = *mots;
		return i;
	}

    return -1;
}

/**
 * initializes the receive mail boxes (16-31)
 */
void Com_CAN_InitReceiveMBoxes() {
	// Mailboxs can be written to 16-bits or 32-bits at a time
	// Write to the MSGID field of TRANSMIT mailboxes MBOX0 - 15
	ECanaMboxes.MBOX16.MSGID.all = Com_CAN_BaseAddress + 0; // 0 - 7 is for value messages
	ECanaMboxes.MBOX17.MSGID.all = Com_CAN_BaseAddress + 1;
	ECanaMboxes.MBOX18.MSGID.all = Com_CAN_BaseAddress + 2;
	ECanaMboxes.MBOX19.MSGID.all = Com_CAN_BaseAddress + 3;
	ECanaMboxes.MBOX20.MSGID.all = Com_CAN_BaseAddress + 4;
	ECanaMboxes.MBOX21.MSGID.all = Com_CAN_BaseAddress + 5;
	ECanaMboxes.MBOX22.MSGID.all = Com_CAN_BaseAddress + 6;
	ECanaMboxes.MBOX23.MSGID.all = Com_CAN_BaseAddress + 7;
	ECanaMboxes.MBOX24.MSGID.all = Com_CAN_BaseAddress + 8;
	ECanaMboxes.MBOX25.MSGID.all = Com_CAN_BaseAddress + 9;
	ECanaMboxes.MBOX26.MSGID.all = Com_CAN_BaseAddress + 10;
	ECanaMboxes.MBOX27.MSGID.all = Com_CAN_BaseAddress + 11;
	ECanaMboxes.MBOX28.MSGID.all = Com_CAN_BaseAddress + 12;
	ECanaMboxes.MBOX29.MSGID.all = Com_CAN_BaseAddress + 13;
	ECanaMboxes.MBOX30.MSGID.all = Com_CAN_BaseAddress + 14;
	ECanaMboxes.MBOX31.MSGID.all = Com_CAN_BaseAddress - 1; // this is for start measurement message

	ECanaMboxes.MBOX16.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX17.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX18.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX19.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX20.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX21.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX22.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX23.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX24.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX25.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX26.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX27.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX28.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX29.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX30.MSGCTRL.bit.DLC = 4;
	ECanaMboxes.MBOX31.MSGCTRL.bit.DLC = 4;

	ECanaRegs.CANMD.all = 0xFFFF0000;
	ECanaRegs.CANMIM.all = 0xFFFF0000;
	ECanaRegs.CANME.all = 0xFFFF0000;
}

/**
 * init transmit message m box (0)
 */
void Com_CAN_InitTransmitMBoxes() {
	struct ECAN_REGS ECanaShadow;

	// transmit measurement message
	ECanaMboxes.MBOX0.MSGID.all = Com_CAN_BoardId;
	ECanaMboxes.MBOX0.MSGCTRL.bit.DLC = 8;

	ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
	ECanaShadow.CANMD.bit.MD0 = 0; // transmit
	ECanaRegs.CANMD.all = ECanaShadow.CANMD.all;

	ECanaShadow.CANMIM.all = ECanaRegs.CANMIM.all;
	ECanaShadow.CANMIM.bit.MIM0 = 1; // receive
	ECanaRegs.CANMIM.all = ECanaShadow.CANMIM.all;

	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	ECanaShadow.CANME.bit.ME0 = 1; // enable
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;

	// start measurement message
	ECanaMboxes.MBOX1.MSGID.all =  Com_CAN_BaseAddress - 1;
	ECanaMboxes.MBOX1.MSGCTRL.bit.DLC = 4;

	ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
	ECanaShadow.CANMD.bit.MD1 = 0; // transmit
	ECanaRegs.CANMD.all = ECanaShadow.CANMD.all;

	ECanaShadow.CANMIM.all = ECanaRegs.CANMIM.all;
	ECanaShadow.CANMIM.bit.MIM1 = 1; // receive
	ECanaRegs.CANMIM.all = ECanaShadow.CANMIM.all;

	ECanaShadow.CANME.all = ECanaRegs.CANME.all;
	ECanaShadow.CANME.bit.ME1 = 1; // enable
	ECanaRegs.CANME.all = ECanaShadow.CANME.all;
}

//EOF
