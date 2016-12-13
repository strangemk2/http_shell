CC=gcc
CFLAGS=-Wall -g
CPP=g++
CPPFLAGS=$(CFLAGS) -std=c++11
LDFLAGS=
OBJS=base64.o crc32.o hs_http.o hs_crypt.o hs_compress.o hs_base64.o
DEPS=hs_exception.h

all: hs

################################################################################

hs: hs_main.cpp $(DEPS) $(OBJS)
	$(CPP) $(CPPFLAGS) hs_main.cpp $(OBJS) -o $@ $(LDFLAGS)

base64.o: base64.c base64.h
	$(CC) $(CFLAGS) base64.c -c -Wno-unused-but-set-variable

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

################################################################################

clean:
	rm -f *.o
	rm -f hs
