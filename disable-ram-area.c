#include <efi.h>
#include <efilib.h>


EFI_STATUS ParseCommandLine(CHAR16 *command_line, EFI_PHYSICAL_ADDRESS *Addr, EFI_PHYSICAL_ADDRESS *EndAddr, UINTN *Stalltime) {
    // Ensure command_line is not null
    if (command_line == NULL) {
        Print(L"No command line arguments provided.\n");
        return EFI_INVALID_PARAMETER;
    }

    Print(L"Command line: %s\n", command_line);

    // Parse the first address (Hex)
    *Addr = 0;
    while (*command_line && *command_line != ' ') {
        if (*command_line >= '0' && *command_line <= '9') {
            *Addr = (*Addr << 4) | (*command_line - '0');
        } else if (*command_line >= 'a' && *command_line <= 'f') {
            *Addr = (*Addr << 4) | (*command_line - 'a' + 10);
        } else if (*command_line >= 'A' && *command_line <= 'F') {
            *Addr = (*Addr << 4) | (*command_line - 'A' + 10);
        }
        command_line++;
    }

    while (*command_line == ' ') command_line++;  // Skip spaces

    // Parse the second address (Hex)
    *EndAddr = 0;
    while (*command_line && *command_line != ' ') {
        if (*command_line >= '0' && *command_line <= '9') {
            *EndAddr = (*EndAddr << 4) | (*command_line - '0');
        } else if (*command_line >= 'a' && *command_line <= 'f') {
            *EndAddr = (*EndAddr << 4) | (*command_line - 'a' + 10);
        } else if (*command_line >= 'A' && *command_line <= 'F') {
            *EndAddr = (*EndAddr << 4) | (*command_line - 'A' + 10);
        }
        command_line++;
    }

    while (*command_line == ' ') command_line++;  // Skip spaces

    // Parse the stall time (Decimal)
    *Stalltime = 0;
    while (*command_line && *command_line >= '0' && *command_line <= '9') {
        *Stalltime = (*Stalltime * 10) + (*command_line - '0');
        command_line++;
    }

    return EFI_SUCCESS;
}

EFI_STATUS
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
    EFI_STATUS Status;
	EFI_LOADED_IMAGE *loaded_image;
    EFI_PHYSICAL_ADDRESS Addr, EndAddr;
    UINTN Stalltime;

   Status = uefi_call_wrapper(BS->HandleProtocol, 3, ImageHandle, &LoadedImageProtocol, (void **)&loaded_image);
    if (EFI_ERROR(Status)) {
       Print(L"Error HandleProtocol: %r\n", Status);
       uefi_call_wrapper(BS->Stall, 1, 5000000);
       return Status;
    }

    CHAR16 *command_line = (CHAR16 *)loaded_image->LoadOptions;
    if (command_line == NULL) {
        Print(L"No command line arguments provided.\n");
        return EFI_INVALID_PARAMETER;
    }
    
    Status = ParseCommandLine(command_line, &Addr, &EndAddr, &Stalltime);
    if (EFI_ERROR(Status)) {
        Print(L"Failed to parse command line: %r\n", Status);
        return Status;
    }

    Print(L"Parsed values - Addr: 0x%lx, EndAddr: 0x%lx, Stalltime: %lu ms\n", Addr, EndAddr, Stalltime);

    Print(L"Disable RAM Area...!\n");

    // Align the starting address (Addr) to the previous page boundary
    Addr = Addr & ~(EFI_PAGE_SIZE - 1);

    // Calculate the number of pages between the aligned Addr and EndAddr
    UINTN NumPages = ((EndAddr - Addr) + EFI_PAGE_SIZE - 1) / EFI_PAGE_SIZE;

    Print(L"Disable from 0x%lx to 0x%lx, which is %lu pages\n\n", Addr, EndAddr, NumPages);

    Status = uefi_call_wrapper(BS->AllocatePages, 4, 2, 8, NumPages, &Addr);

    if (EFI_ERROR(Status)) {
        Print(L"Error!\n");
        Print(L"AllocatePages failed: %r\n", Status);
        uefi_call_wrapper(BS->Stall, 1, Stalltime * 1000);
        return Status;  // Return the error status
    } else {
        Print(L"Success!\n");
        Print(L"Memory allocated at address: 0x%lx for %lu pages\n", Addr, NumPages);
    }

    uefi_call_wrapper(BS->Stall, 1, Stalltime * 1000);

    Exit(EFI_SUCCESS, 0,0);
    return EFI_SUCCESS;
}
