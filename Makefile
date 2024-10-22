ARCH	= x86_64

TARGET  = disable-ram-area.efi
FORMAT  = efi-app-$(ARCH)

INC		 = /usr/include/efi
CFLAGS   = -I$(INC) -I$(INC)/$(ARCH) \
		   -fPIC -fshort-wchar -ffreestanding \
		   -fno-stack-protector -Wall -D$(ARCH) -Werror -m64 -mno-red-zone

ifeq ($(ARCH),x86_64)
  CFLAGS += -DEFI_FUNCTION_WRAPPER
endif

LDFLAGS = -T /usr/lib/elf_$(ARCH)_efi.lds -Bsymbolic -shared -nostdlib -znocombreloc \
		  /usr/lib/crt0-efi-$(ARCH).o

OBJS = disable-ram-area.o

%.efi: %.so
	objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym -j .rel \
			-j .rela -j .reloc -S --target=$(FORMAT) $^ $@

%.so: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^ $(shell $(CC) $(CFLAGS) -print-libgcc-file-name) \
		/usr/lib/libgnuefi.a /usr/lib/libefi.a

all: $(TARGET)

clean:
	rm -f $(TARGET) *.so $(OBJS)
