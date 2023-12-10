CC		= xcrun -sdk iphoneos gcc

OBJCOPY	= /opt/homebrew/opt/binutils/bin/gobjcopy

CFLAGS	= -Iinclude/ -target armv7-apple-ios10.0
CFLAGS	+= -Wall -Wno-incompatible-library-redeclaration -fno-stack-protector -nostdlib -static -nostdlibinc -Wl,-preload -Wl,-no_uuid
CFLAGS	+= -Wl,-e,start -Wl,-order_file,sym_order.txt -Wl,-image_base,0x10078000 -Wl,-sectalign,__DATA,__common,0x4 -Wl,-segalign,0x4

OBJ		= jump

SOURCE	= jump.c
			
.PHONY: all

all:
	gcc cut.c -o cut
	$(CC) entry.S $(SOURCE) $(CFLAGS) -o $(OBJ).o
	../vmacho -fM 0x80000 $(OBJ).o $(OBJ).prebin
	./cut $(OBJ).prebin $(OBJ).bin
	xxd -i $(OBJ).bin $(OBJ).h
	-$(RM) $(OBJ).o
	-$(RM) $(OBJ).prebin
	
clean:
	-$(RM) $(ASMSOURCE)
	-$(RM) $(OBJ).o
	-$(RM) $(OBJ).bin
	-$(RM) $(OBJ).prebin
	-$(RM) $(OBJ).h
	-$(RM) cut
	
