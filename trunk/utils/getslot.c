/* $Id$ */

/*
 * Copyright (c) 2007 John Dickinson.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/types.h>
#include <security/cryptoki.h>
#include <security/pkcs11.h>

void
main(int argc, char **argv)
{
	CK_RV	rv;
	CK_MECHANISM genmech;
	CK_SESSION_HANDLE hSession;
	CK_SESSION_INFO sessInfo;
	CK_SLOT_ID_PTR pSlotList = NULL_PTR;
	CK_SLOT_ID SlotID;
	CK_ULONG ulSlotCount = 0;
	CK_MECHANISM_INFO mech_info;
	int i = 0;

	CK_OBJECT_HANDLE privatekey, publickey;

	/* Initialize the CRYPTOKI library */
	rv = C_Initialize(NULL_PTR);

	if (rv != CKR_OK) {
		fprintf(stderr, "C_Initialize: Error = 0x%.8X\n", rv);
		exit(1);
	}

	/* Get slot list for memory alloction */
	rv = C_GetSlotList(0, NULL_PTR, &ulSlotCount);

	if ((rv == CKR_OK) && (ulSlotCount > 0)) {
		fprintf(stdout, "slotCount = %d\n", ulSlotCount);
		pSlotList = malloc(ulSlotCount * sizeof (CK_SLOT_ID));

		if (pSlotList == NULL) {
			fprintf(stderr, "System error: unable to allocate "
			    "memory\n");
			exit(1);
		}

		/* Get the slot list for processing */
		rv = C_GetSlotList(0, pSlotList, &ulSlotCount);
		if (rv != CKR_OK) {
			fprintf(stderr, "GetSlotList failed: unable to get "
			    "slot count\n");
			goto cleanup;
		}
	} else {
		fprintf(stderr, "GetSlotList failed: unable to get slot "
		    "list\n");
		exit(1);
	}

	/* Print slot info */
	for (i = 0; i < ulSlotCount; i++) {
		SlotID = pSlotList[i];
		fprintf(stdout, "Slot found: %d - ", SlotID);
		CK_SLOT_INFO Info;
		CK_TOKEN_INFO TInfo;
		rv = C_GetSlotInfo(SlotID, &Info);
		fprintf(stdout, "Slot Description: %s\n", Info.slotDescription);
		fprintf(stdout, "Slot Flags: 0x%.8X\n", Info.flags);
		/* Get Token info for each slot */
		rv = C_GetTokenInfo(SlotID, &TInfo);
		fprintf(stdout, "Token Label: %s\n", TInfo.label);
		fprintf(stdout, "Token Flags: 0x%.8X\n", TInfo.flags);
		fprintf(stdout, "Token manufacturerID: %s\n", TInfo.manufacturerID);
		fprintf(stdout, "Token model: %s\n", TInfo.model);
		fprintf(stdout, "Token serialNumber: %s\n", TInfo.serialNumber);
		fprintf(stdout, "Token ulMaxSessionCount: %ld\n", TInfo.ulMaxSessionCount);
		fprintf(stdout, "Token ulSessionCount: %ld\n", TInfo.ulSessionCount);
		fprintf(stdout, "Token ulMaxRwSessionCount: %ld\n", TInfo.ulMaxRwSessionCount);
		fprintf(stdout, "Token ulRwSessionCount: %ld\n", TInfo.ulRwSessionCount);
		fprintf(stdout, "Token ulMaxPinLen: %ld\n", TInfo.ulMaxPinLen);
		fprintf(stdout, "Token ulMinPinLen: %ld\n", TInfo.ulMinPinLen);
		fprintf(stdout, "Token ulTotalPublicMemory: %ld\n", TInfo.ulTotalPublicMemory);
		fprintf(stdout, "Token ulFreePublicMemory: %ld\n", TInfo.ulFreePublicMemory);
		fprintf(stdout, "Token ulTotalPrivateMemory: %ld\n", TInfo.ulTotalPrivateMemory);
		fprintf(stdout, "Token ulFreePrivateMemory: %ld\n", TInfo.ulFreePrivateMemory);
		fprintf(stdout, "Token hardwareVersion: %d\n", TInfo.hardwareVersion);
		fprintf(stdout, "Token firmwareVersion: %d\n", TInfo.firmwareVersion);
		fprintf(stdout, "Token utcTime: %c\n", TInfo.utcTime);		

		fprintf(stdout, "\n");
	}

cleanup:
	if (pSlotList)
		free(pSlotList);
}

