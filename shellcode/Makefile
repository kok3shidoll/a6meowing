CC		= xcrun -sdk iphoneos gcc

OBJCOPY	= /opt/homebrew/opt/binutils/bin/gobjcopy

CFLAGS	= -Iinclude/ -target armv7-apple-ios10.0
CFLAGS	+= -Wall -Wno-incompatible-library-redeclaration -fno-stack-protector -nostdlib -static -nostdlibinc -Wl,-preload -Wl,-no_uuid
CFLAGS	+= -Wl,-e,start -Wl,-order_file,sym_order.txt -Wl,-image_base,0x10000000 -Wl,-sectalign,__DATA,__common,0x4 -Wl,-segalign,0x4

OBJ		= payload

SOURCE	= entryp.c
			
.PHONY: all

all:
	cd jump && make
	cd patch && make
	gcc marge.c -o marge
	$(CC) entry.S $(SOURCE) $(CFLAGS) -o pre_$(OBJ).o
	./vmacho -fM 0x80000 pre_$(OBJ).o pre_$(OBJ).bin
	-$(RM) pre_$(OBJ).o
	./marge pre_$(OBJ).bin patch/$(OBJ).bin $(OBJ).bin
	xxd -i $(OBJ).bin $(OBJ).h
	-$(RM) pre_$(OBJ).bin
	
clean:
	cd jump && make clean
	cd patch && make clean
	-$(RM) marge
	-$(RM) $(OBJ).o
	-$(RM) $(OBJ).bin
	-$(RM) $(OBJ).h
	-$(RM) pre_$(OBJ).o
	-$(RM) pre_$(OBJ).bin
	
