/*
 * Copyright (c) 2011, Intel Corporation
 * Modified by :Puck Meerburg
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
#include "mouse.h"

static EFI_GUID SimplePointerProtocolGUID = EFI_SIMPLE_POINTER_PROTOCOL_GUID;

void printInt(SIMPLE_TEXT_OUTPUT_INTERFACE *conOut, int value) {
	CHAR16 out[32];
	CHAR16 *ptr = out;
	if (value == 0) {
		conOut->OutputString(conOut, L"0");
		return;
	}

	ptr += 31;
	*--ptr = 0;
	int tmp = value;// >= 0 ? value : -value; 
	
	while (tmp) {
		*--ptr = '0' + tmp % 10;
		tmp /= 10;
	}
	if (value < 0) *--ptr = '-';
	conOut->OutputString(conOut, ptr);
}

/**
 * efi_main - The entry point for the EFI application
 * @image: firmware-allocated handle that identifies the image
 * @SystemTable: EFI system table
 */
EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systemTable)
{
	EFI_BOOT_SERVICES *bs = systemTable->BootServices;
	SIMPLE_TEXT_OUTPUT_INTERFACE *conOut = systemTable->ConOut;
	EFI_SIMPLE_POINTER_PROTOCOL *protocol;
	EFI_STATUS status = bs->LocateProtocol(&SimplePointerProtocolGUID, NULL, (void**)&protocol);
	if (EFI_ERROR(status) || protocol == NULL) {
		conOut->OutputString(conOut, L"Failed to init pointer!\r\n");
		return status;
	}
	EFI_EVENT *mouseEvent = &protocol->WaitForInput;
	UINTN indie;
	while(bs->WaitForEvent(1, mouseEvent, &indie) == EFI_SUCCESS) {
		EFI_SIMPLE_POINTER_STATE state;
		protocol->GetState(protocol, &state);
		conOut->OutputString(conOut, L"Current X: ");
		printInt(conOut, state.RelativeMovementX);
		conOut->OutputString(conOut, L", Current Y: ");
		printInt(conOut, state.RelativeMovementY);
		conOut->OutputString(conOut, L"\r\n");
	}
	return EFI_SUCCESS;
}
