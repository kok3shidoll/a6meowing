MACOSX_CC		= xcrun -sdk macosx gcc

CFLAGS			= -I./include -Wall
CFLAGS			+= -O0

LDFLAGS			= -framework IOKit -framework CoreFoundation

MACOSX_OBJ		= a6meowing

CODESIGN	= codesign -f -s - --entitlements ent.xml
STRIP		= strip

SOURCE		= io/iousb.c common/common.c
			
EXPLOIT		= a6meow.c

.PHONY: all clean

all:
	cd shellcode && make
	$(MACOSX_CC) $(CFLAGS) $(LDFLAGS) $(SOURCE) main.c $(EXPLOIT) -o $(MACOSX_OBJ)

clean:
	cd shellcode && make clean
	-$(RM) $(MACOSX_OBJ)
