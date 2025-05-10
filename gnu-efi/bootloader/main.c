#include <efi.h>
#include <efilib.h>
#include <elf.h>

typedef unsigned long long size_t;

//Function to load a file from the system
//UEFI treats directory as file hence why we need to pass the directory as well
EFI_FILE* loadFile(EFI_FILE* dir, CHAR16* path, EFI_HANDLE imageHandle, EFI_SYSTEM_TABLE* systab){

  //Every UEFI function needs the system table and image handle
  EFI_FILE* LoadedFile;
		//Get the loaded image protocol from the image handle
        EFI_LOADED_IMAGE_PROTOCOL* LoadedImage;
        systab->BootServices->HandleProtocol(imageHandle, &gEfiLoadedImageProtocolGuid, (VOID**)&LoadedImage); //Get the loaded image protocol from the image handle

        //Get the file system protocol from the device handle
        EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* FileSystem;
        systab->BootServices->HandleProtocol(LoadedImage->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (VOID**)&FileSystem); //Get the file system protocol from the device handle

        //Open the directory if it is not already open
        if (dir == NULL) {
          FileSystem->OpenVolume(FileSystem, &dir);
        }

		//Open the file
        EFI_STATUS s = dir->Open(dir, &LoadedFile, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
		if(s != EFI_SUCCESS){
            return NULL;
		}
        return LoadedFile;
}

int memcmp(const void* p1, const void* p2, size_t n) { //This is scanning through 2 buffers of memory and scanning for the size of n
	const unsigned char* a = p1, *b = p2;
	for (size_t i = 0; i < n; ++i) {
		if (a[i] < b[i])
			return -1;
		else if (a[i] > b[i])
			return 1;
	}
	return 0;
} //memcmp function compares 2 buffers of memory

EFI_STATUS efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable) {
  	InitializeLib(ImageHandle, SystemTable);
    Print(L"Fuck you i hate yo ass!\n\r");


	EFI_FILE* kernel = loadFile(NULL, L"kernel.elf", ImageHandle, SystemTable);

    if(kernel == NULL){
    	Print(L"F4il3d to l04d k3rn3l grrrr\n\r");
    }
    else{
      Print(L"Kernel loaded!!! omg yay! :3 \n\r");
     }

     Elf64_Ehdr header;
     {
       UINTN FileInfoSize;
       EFI_FILE_INFO* FileInfo;
       kernel->GetInfo(kernel, &gEfiFileInfoGuid, &FileInfoSize, &FileInfo); 	//Set FileInfoSize to the size of the buffer
       SystemTable->BootServices->AllocatePool(EfiLoaderData, FileInfoSize, (VOID**)&FileInfo); //Allocate a buffer of FileInfoSize bytes
       kernel->GetInfo(kernel, &gEfiFileInfoGuid, &FileInfoSize, (VOID**)&FileInfo); //Get the file info

       UINTN size = sizeof(header); //this is the size of the header
       kernel->Read(kernel, &size, &header); //This is the actual thing that reads the file content
     }

	if(
		memcmp(&header.e_ident[EI_MAG0], ELFMAG, SELFMAG) != 0 ||  // Magic number check
		header.e_ident[EI_CLASS] != ELFCLASS64 ||                  // 64-bit format check
		header.e_ident[EI_DATA] != ELFDATA2LSB ||                  // Little-endian check
		header.e_type != ET_EXEC ||                                // Executable file type check
		header.e_machine != EM_X86_64 ||                           // x86_64 architecture check
		header.e_version != EV_CURRENT                             // ELF version check
	)

     {
       Print(L"Kernel is bad you bad boy >:( \r\n");
     }
     else
     {
       Print(L"Kernel is good you good boy >:D \r\n");
     }


	return EFI_SUCCESS; // Exit the UEFI application
}
