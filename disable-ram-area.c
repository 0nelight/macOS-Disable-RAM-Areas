#include <efibind.h>
#include <efidef.h>
#include <efidevp.h>
#include <eficon.h>
#include <efiprot.h>
#include <efiapi.h>
#include <efierr.h>


EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE *systemTable) {
    SIMPLE_TEXT_OUTPUT_INTERFACE *conOut = systemTable->ConOut;

    conOut->OutputString(conOut, L"Disable Ram Area...\r\n");

    EFI_PHYSICAL_ADDRESS lowest_error_address = 0x300000000;
    EFI_PHYSICAL_ADDRESS highest_error_address = 0x309FFFF48;

    EFI_BOOT_SERVICES *gBS;
    gBS = systemTable->BootServices;

    UINTN num_pages = (highest_error_address - lowest_error_address + 4095) / 4096;

    EFI_STATUS status = gBS->AllocatePages(AllocateAddress, EfiRuntimeServicesData, num_pages, &lowest_error_address);
    if (EFI_ERROR(status)) {
        conOut->OutputString(conOut, L"Failed to allocate pages!\r\n");
        return status;
    }

    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");
    conOut->OutputString(conOut, L"Disabled Ram Area!\r\n");

    gBS->Stall(3000000); // Stall for 3 seconds

    return EFI_SUCCESS;
}
