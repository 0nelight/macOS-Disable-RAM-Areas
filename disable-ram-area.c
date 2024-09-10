#include <efibind.h>
#include <efidef.h>
#include <efidevp.h>
#include <eficon.h>
#include <efiprot.h>
#include <efiapi.h>
#include <efierr.h>

// Define the GUID for EFI_LOADED_IMAGE_PROTOCOL
static EFI_GUID LoadedImageProtocol = EFI_LOADED_IMAGE_PROTOCOL_GUID;

#define DEFAULT_STALL_TIME_MS 1000

EFI_STATUS efi_main(EFI_HANDLE image_handle, EFI_SYSTEM_TABLE *system_table) {
    EFI_STATUS status;
    EFI_LOADED_IMAGE *loaded_image;
    EFI_BOOT_SERVICES *boot_services = system_table->BootServices;
    EFI_SIMPLE_TEXT_OUT_PROTOCOL *console_output = system_table->ConOut;

    // Call HandleProtocol directly for EFI 1.1 to retrieve the loaded image protocol
    status = boot_services->HandleProtocol(image_handle, &LoadedImageProtocol, (void **)&loaded_image);
    if (EFI_ERROR(status)) {
        // Create a simple error message
        CHAR16 error_message[] = L"Failed to get loaded image protocol. Halting.\r\n";
        console_output->OutputString(console_output, error_message);

        boot_services->Stall(DEFAULT_STALL_TIME_MS * 1000);
        return status;
    }

    // Check if there are command-line parameters
    if (loaded_image->LoadOptionsSize > 0 && loaded_image->LoadOptions != NULL) {
        CHAR16 *cmdline = (CHAR16 *)loaded_image->LoadOptions;

        // Output the command line arguments
        CHAR16 cmdline_message[] = L"Received command line arguments:\r\n";
        console_output->OutputString(console_output, cmdline_message);
        console_output->OutputString(console_output, cmdline);  // Print the command-line options
        console_output->OutputString(console_output, L"\r\n");
    } else {
        CHAR16 no_params_message[] = L"No command line arguments received.\r\n";
        console_output->OutputString(console_output, no_params_message);
    }

    // Success message (optional)
    CHAR16 success_message[] = L"Loaded image protocol retrieved successfully.\r\n";
    console_output->OutputString(console_output, success_message);

    boot_services->Stall(DEFAULT_STALL_TIME_MS * 1000);
    
    return EFI_SUCCESS;
}

