CC		= xcrun -sdk iphoneos gcc

OBJCOPY	= /opt/homebrew/opt/binutils/bin/gobjcopy

CFLAGS	= -Iinclude/ -Iinclude/drivers/ -DDER_TAG_SIZE=8 -target arm64-apple-ios12.0
CFLAGS	+= -Wall -Wno-incompatible-library-redeclaration -fno-stack-protector -nostdlib -static -nostdlibinc -Wl,-preload -Wl,-no_uuid
CFLAGS	+= -Wl,-e,start -Wl,-order_file,sym_order.txt -Wl,-image_base,0x180018000 -Wl,-sectalign,__DATA,__common,0x8 -Wl,-segalign,0x8

OBJ		= payload

SOURCE	=	\
			payload.c \
			offsetfinder.c \
			lib/memset.c \
			lib/memmem.c \
			lib/memcmp.c \
			lib/memmove.c \
			
.PHONY: all

all:
	$(CC) entry.S $(SOURCE) $(CFLAGS) -o $(OBJ).o
	#./vmacho -fM 0x80000 $(OBJ).o $(OBJ).bin
	
clean:
	-$(RM) $(ASMSOURCE)
	-$(RM) $(OBJ).o
	#-$(RM) $(OBJ).bin
	
