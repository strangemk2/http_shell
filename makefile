CC=gcc
CFLAGS=-Wall -s -Os
CPP=g++
CPPFLAGS=$(CFLAGS) -std=c++11
LDFLAGS=-Luzlib/lib -ltinf tiny-AES128-C/aes.o
OBJS=base64.o crc32.o md5.o hs_http.o hs_crypt.o hs_compress.o hs_base64.o hs_misc.o
DEPS=hs_exception.h

.PHONY: uzlib tiny_aes

all: uzlib tiny_aes hs

################################################################################

hs: hs_main.cpp $(DEPS) $(OBJS)
	$(CPP) $(CPPFLAGS) hs_main.cpp $(OBJS) -o $@ $(LDFLAGS)

base64.o: base64.c base64.h
	$(CC) $(CFLAGS) base64.c -c -Wno-unused-but-set-variable

md5.o: md5.c md5.h
	$(CC) $(CFLAGS) md5.c -c

crc32.o: crc32.c crc32.h
	$(CC) $(CFLAGS) crc32.c -c

hs_http.o: hs_http.cpp hs_http.h $(DEPS)
	$(CPP) $(CPPFLAGS) hs_http.cpp -c -Ihttp-client-c/src

hs_crypt.o: hs_crypt.cpp hs_crypt.h $(DEPS)
	$(CPP) $(CPPFLAGS) hs_crypt.cpp -c -Itiny-AES128-C

hs_compress.o: hs_compress.cpp hs_compress.h $(DEPS)
	$(CPP) $(CPPFLAGS) hs_compress.cpp -c -Iuzlib/src

hs_base64.o: hs_base64.cpp hs_base64.h $(DEPS)
	$(CPP) $(CPPFLAGS) hs_base64.cpp -c

hs_misc.o: hs_misc.cpp hs_misc.h $(DEPS)
	$(CPP) $(CPPFLAGS) hs_misc.cpp -c

uzlib:
	$(MAKE) -C uzlib/src -f makefile.elf

tiny_aes:
	$(MAKE) -C tiny-AES128-C

################################################################################

clean:
	$(MAKE) -C uzlib/src -f makefile.elf clean
	$(MAKE) -C tiny-AES128-C clean
	rm -f *.o
	rm -f hs
