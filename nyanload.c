/*
 * Copyright (c) 2011, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer
 *      in the documentation and/or other materials provided with the
 *      distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */
 
#include "efibind.h"
#include "efidef.h"
#include "efidevp.h"
#include "eficon.h"
#include "efiapi.h"
#include "efierr.h"
#include "efiprot.h"
#include "nyan.h"
#include "nyanbutt1.h"

static EFI_GUID GraphicsOutputProtocolGUID = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;

/**
 * efi_main - The entry point for the EFI application
 * @image: firmware-allocated handle that identifies the image
 * @SystemTable: EFI system table
 */
EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systemTable) {
	EFI_BOOT_SERVICES *bs = systemTable->BootServices;
	EFI_STATUS status;
	EFI_GRAPHICS_OUTPUT_PROTOCOL *graphicsProtocol;
	SIMPLE_TEXT_OUTPUT_INTERFACE *conOut = systemTable->ConOut;
	EFI_GRAPHICS_OUTPUT_MODE_INFORMATION *info;
	UINTN SizeOfInfo, sWidth, sHeight;
	static EFI_INPUT_KEY keys[10]; //up  , up  , down, down, left,right, left,right, b, a
	int keysPos = 0;

	status = bs->LocateProtocol(&GraphicsOutputProtocolGUID, NULL, 
		(void**)&graphicsProtocol);

	if (EFI_ERROR(status) || graphicsProtocol == NULL) {
		conOut->OutputString(conOut, L"Failed to init gfx!\r\n");
		return status;
	}

	//Switch to current mode so gfx is started.
	status = graphicsProtocol->SetMode(graphicsProtocol, graphicsProtocol->Mode->Mode);
	if (EFI_ERROR(status)) {
		conOut->OutputString(conOut, L"Failed to set default mode!\r\n");
		return status;
	}

	EFI_GRAPHICS_OUTPUT_BLT_PIXEL p;
	p.Red = 15;
	p.Green = 77;
	p.Blue = 143;
	graphicsProtocol->QueryMode(graphicsProtocol, graphicsProtocol->Mode->Mode, &SizeOfInfo, &info);
	sWidth = info->HorizontalResolution;
	sHeight = info->VerticalResolution;	
	status = graphicsProtocol->Blt(graphicsProtocol, &p, EfiBltVideoFill, 0, 0, 0, 0, sWidth, sHeight, 0);

	/* disable efi watchdog timer */
	bs->SetWatchdogTimer(0, 0, 0, NULL);

	/* experimental disk loading */
	
	/* ------------ end -------- */
	
	/* experimental graphics draw */

        // nyan
        int SCALE = 6;
	int i, j;
	int xtopleft = sWidth/2 - 35*(SCALE/2), ytopleft = sHeight/2 - 20*(SCALE/2);
	int offset = 0;
	static EFI_GRAPHICS_OUTPUT_BLT_PIXEL rainbow[] = {{17,18,255,0x00},{11,168,255,0x00},{0,0xFF,0xFF,0x00},
{12,0xFF,74,0x00},{255,174,15,0x00},{255,68,118,0x00}};

	for (i = 10; i < 35; i++) { // x
		for (j = 0; j < 20; j++) { // y
			status = graphicsProtocol->Blt(graphicsProtocol,
			&nyancat[j*35+i], EfiBltVideoFill, 0, 0, xtopleft+i*SCALE, ytopleft+j*SCALE, SCALE, SCALE, 0);
		}
	}

while (1) {
	// clear old
	status = graphicsProtocol->Blt(graphicsProtocol, &p, EfiBltVideoFill, 0, 0, 0, ytopleft, xtopleft+10*SCALE, SCALE*20, 0);
        // rainbow
	for (i = 0; i < xtopleft+(7*SCALE); i++) { // x
		for (j = 0; j < 6; j++) { // y
			status = graphicsProtocol->Blt(graphicsProtocol,
			&rainbow[j], EfiBltVideoFill, 0, 0, i, ytopleft+j*SCALE*3+offset*SCALE, SCALE, SCALE*3, 0);
		}
		if (i % (9*SCALE) == 0) {
			offset = !offset;
		}
	}

	if (offset) {
		for (i = 0; i < 10; i++) { // x
			for (j = 0; j < 20; j++) { // y
				if (nyancat[j*35+i].Red != p.Red && nyancat[j*35+i].Green != p.Green && nyancat[j*35+i].Blue != p.Blue)
				status = graphicsProtocol->Blt(graphicsProtocol,
				&nyancat[j*35+i], EfiBltVideoFill, 0, 0, xtopleft+i*SCALE, ytopleft+j*SCALE, SCALE, SCALE, 0);
			}
		}
	} else {
		for (i = 0; i < 10; i++) { // x
			for (j = 0; j < 20; j++) { // y
				if (nyanbutt1[j*10+i].Red != p.Red && nyanbutt1[j*10+i].Green != p.Green && nyanbutt1[j*10+i].Blue != p.Blue)
				status = graphicsProtocol->Blt(graphicsProtocol,
				&nyanbutt1[j*10+i], EfiBltVideoFill, 0, 0, xtopleft+i*SCALE, ytopleft+j*SCALE, SCALE, SCALE, 0);
			}
		}
	}
	status = systemTable->ConIn->ReadKeyStroke(systemTable->ConIn, &keys[keysPos]);

	if (status == EFI_SUCCESS) {
		// check if user has correctly entered konami code!
		if (keys[keysPos].UnicodeChar == 'a') {
		for (i = 1; i < 11; i++) {
			switch(i-1) {
			// up, up
			case 0:
			case 1:
			if (keys[(i+keysPos)%10].ScanCode != 0x01) goto sad;
			break;
			// down, down
			case 2:
			case 3:
			if (keys[(i+keysPos)%10].ScanCode != 0x02) goto sad;
			break;
			// left (x2)
			case 4:
			case 6:
			if (keys[(i+keysPos)%10].ScanCode != 0x04) goto sad;
			break;
			// right (x2)
			case 5:
			case 7:
			if (keys[(i+keysPos)%10].ScanCode != 0x03) goto sad;
			break;
			// a
			case 9:
			if (keys[(i+keysPos)%10].UnicodeChar != 'a') goto sad;
			break;
			// b
			case 8:
			if (keys[(i+keysPos)%10].UnicodeChar != 'b') goto sad;
			break;
			}

		}
		break;
		}
		sad:
		keysPos++;
		keysPos = keysPos % 10;

	}
	bs->Stall(500000);
}
	
	/* re-enable efi watchdog timer - because you're supposed to! :o */
	bs->SetWatchdogTimer(1337, 0, 0, NULL);

	/* ------------ end -------- */

	return EFI_SUCCESS;
}
