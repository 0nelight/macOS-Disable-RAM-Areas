#include <efibind.h>
#include <efidef.h>
#include <efidevp.h>
#include <eficon.h>
#include <efiprot.h>
#include <efiapi.h>
#include <efierr.h>

EFI_STATUS
efi_main(EFI_HANDLE image, EFI_SYSTEM_TABLE * systemTable) {
  SIMPLE_TEXT_OUTPUT_INTERFACE * conOut = systemTable -> ConOut;

  conOut -> OutputString(conOut, L"Set Bad Ram...\r\n");

  EFI_PHYSICAL_ADDRESS Addr = 12884901888;

  EFI_BOOT_SERVICES * gBS;
  gBS = systemTable -> BootServices;

  gBS -> AllocatePages(2, 8, 40960, & Addr);
  
  gBS->Stall(1000000);

  conOut -> OutputString(conOut, L"ok!\r\n");
  
  gBS->Stall(1000000);

  return EFI_SUCCESS;
}
