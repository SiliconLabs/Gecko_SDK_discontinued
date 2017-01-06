LWIP_DIR = ../../../..
CONTRIB_DIR = ../../..
CC = i686-pc-mingw32-gcc
CPPFLAGS = -I . -I $(LWIP_DIR)/src/include/ -I $(LWIP_DIR)/src/include/ipv4/ -I $(CONTRIB_DIR)/ports/win32/include/ 

makefsdata.exe: makefsdata.c
	$(CC) $(CPPFLAGS) $^ -o $@

clean:
	rm makefsdata.exe
