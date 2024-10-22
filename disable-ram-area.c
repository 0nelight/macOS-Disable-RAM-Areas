#include <efi.h>
#include <efilib.h>

struct ADDRESS_RANGE
{
  EFI_PHYSICAL_ADDRESS startAddress;
  UINT64 pageCount;
  struct ADDRESS_RANGE *next;
};

EFI_FILE_HANDLE GetVolume(EFI_HANDLE image)
{
  EFI_LOADED_IMAGE *loaded_image = NULL;                  /* image interface */
  EFI_GUID lipGuid = EFI_LOADED_IMAGE_PROTOCOL_GUID;      /* image interface GUID */
  EFI_FILE_IO_INTERFACE *IOVolume;                        /* file system interface */
  EFI_GUID fsGuid = EFI_SIMPLE_FILE_SYSTEM_PROTOCOL_GUID; /* file system interface GUID */
  EFI_FILE_HANDLE volumeHandle;                                 /* the volume's interface */

  /* get the loaded image protocol interface for our "image" */
  uefi_call_wrapper(BS->HandleProtocol, 3, image, &lipGuid, (void **) &loaded_image);
  /* get the volume handle */
  uefi_call_wrapper(BS->HandleProtocol, 3, loaded_image->DeviceHandle, &fsGuid, (VOID*)&IOVolume);
  uefi_call_wrapper(IOVolume->OpenVolume, 2, IOVolume, &volumeHandle);
  return volumeHandle;
}

UINT64 GetFileSize(EFI_FILE_HANDLE FileHandle)
{
  UINT64 ret;
  EFI_FILE_INFO       *FileInfo;         /* file information structure */
  /* get the file's size */
  FileInfo = LibFileInfo(FileHandle);
  ret = FileInfo->FileSize;
  FreePool(FileInfo);
  return ret;
}

EFI_FILE_HANDLE GetBadRamFileHandle(EFI_FILE_HANDLE volumeHandle, CHAR16 *fileName, EFI_FILE_HANDLE fileHandle)
{
  uefi_call_wrapper(volumeHandle->Open, 5, volumeHandle, &fileHandle, fileName, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY | EFI_FILE_HIDDEN | EFI_FILE_SYSTEM);
  return fileHandle;
}

UINT8* ReadBadRamFile(EFI_FILE_HANDLE fileHandle)
{
  UINT64 fileSize = GetFileSize(fileHandle);
  UINT8* fileBuffer = AllocatePool(fileSize);
  uefi_call_wrapper(fileHandle->Read, 3, fileHandle, &fileSize, fileBuffer);
  return fileBuffer;
}


struct ADDRESS_RANGE* ParseCommandLine(CHAR16 *command_line, UINTN *Stalltime) {
  // Ensure command_line is not null
  if (command_line == NULL) {
      Print(L"No command line arguments provided.\n");
      return 0x0;
  }
  struct ADDRESS_RANGE *initialAddressRange = 0x0;
  UINT64 StartAddr = 0x0;
  UINT64 EndAddr = 0x0;


  Print(L"Command line: %s\n", command_line);

  // Count the number of arguments
  int arg_count = 0;
  CHAR16 *ptr = command_line;
  while (*ptr) {
      while (*ptr == ' ') ptr++;  // Skip spaces
      if (*ptr) arg_count++;      // Found an argument
      while (*ptr && *ptr != ' ') ptr++;  // Move to next space or end
  }
  
  switch (arg_count)
  { 
    // If 4 arguments are provided, we are probably getting
    // the name of the EFI executable as the first one. We
    // can just skip that.
    case 4:
      while (*command_line && *command_line != ' ') command_line++;  // Skip first argument
      while (*command_line == ' ') command_line++;  // Skip spaces after the first argument
      // Fall through, we can keep parsing the command line.
    case 3:
      // Parse the first address (Hex)
      StartAddr = 0;
      while (*command_line && *command_line != ' ') {
          if (*command_line >= '0' && *command_line <= '9') {
              StartAddr = (StartAddr << 4) | (*command_line - '0');
          } else if (*command_line >= 'a' && *command_line <= 'f') {
              StartAddr = (StartAddr << 4) | (*command_line - 'a' + 10);
          } else if (*command_line >= 'A' && *command_line <= 'F') {
              StartAddr = (StartAddr << 4) | (*command_line - 'A' + 10);
          }
          command_line++;
      }
    
      while (*command_line == ' ') command_line++;  // Skip spaces
    
      // Parse the second address (Hex)
      EndAddr = 0;
      while (*command_line && *command_line != ' ') {
          if (*command_line >= '0' && *command_line <= '9') {
              EndAddr = (EndAddr << 4) | (*command_line - '0');
          } else if (*command_line >= 'a' && *command_line <= 'f') {
              EndAddr = (EndAddr << 4) | (*command_line - 'a' + 10);
          } else if (*command_line >= 'A' && *command_line <= 'F') {
              EndAddr = (EndAddr << 4) | (*command_line - 'A' + 10);
          }
          command_line++;
      }

      initialAddressRange = AllocatePool(sizeof(struct ADDRESS_RANGE));
      initialAddressRange->next = 0x0;

      StartAddr /= EFI_PAGE_SIZE;
      EndAddr /= EFI_PAGE_SIZE;
      initialAddressRange->pageCount = EndAddr - StartAddr + 1;
      StartAddr *= EFI_PAGE_SIZE;

      initialAddressRange->startAddress = StartAddr;
    
      while (*command_line == ' ') command_line++;  // Skip spaces
    
      // Parse the stall time (Decimal)
      *Stalltime = 0;
      while (*command_line && *command_line >= '0' && *command_line <= '9') {
          *Stalltime = (*Stalltime * 10) + (*command_line - '0');
          command_line++;
      }
      break;
  }


  return initialAddressRange;
}


EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  EFI_STATUS Status;
  EFI_LOADED_IMAGE *loaded_image;
  UINTN Stalltime;
  struct ADDRESS_RANGE *addressRanges;

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

  addressRanges = ParseCommandLine(command_line, &Stalltime);

  if (addressRanges == 0x0) {
      Print(L"Failed to parse command line.\n");
      return EFI_INVALID_PARAMETER;
  }
  
  while (addressRanges != 0x0)
  {

    Print(L"Parsed values - Addr: 0x%lx, pageNum: %lu, Stalltime: %lu ms\n", addressRanges->startAddress, addressRanges->pageCount, Stalltime);
  
    Print(L"Disable RAM Area...!\n");
  
    Status = uefi_call_wrapper(BS->AllocatePages, 4, 2, 8, addressRanges->pageCount, &(addressRanges->startAddress));
  
    if (EFI_ERROR(Status)) {
        Print(L"Error!\n");
        Print(L"AllocatePages failed: %r\n", Status);
        uefi_call_wrapper(BS->Stall, 1, Stalltime * 1000);
        return Status;  // Return the error status
    } else {
        Print(L"Success!\n");
        Print(L"Memory allocated at address: 0x%lx for %lu pages\n", addressRanges->startAddress, addressRanges->pageCount);
    }

    Print(L"Checking for more address ranges...\n");

    addressRanges = addressRanges->next;
    //addressRanges should be null when it runs out of stuff.
  }

  uefi_call_wrapper(BS->Stall, 1, Stalltime * 1000);

  Exit(EFI_SUCCESS, 0,0);
  return EFI_SUCCESS;
}
