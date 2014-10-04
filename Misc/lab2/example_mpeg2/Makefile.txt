CC=gcc

INCLUDE_DIRS = -I.

LIB_DIRS =

LARGEFILE_FLAGS= -DNO_LARGE_FILE_SUPPORT
#LARGEFILE_FLAGS= -D_FILE_OFFSET_BITS=64 -DLARGE_FILE_SUPPORT -D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE

# Use NDEBUG to turn off asserts and various other conditional compile options
CDEFS= $(LARGEFILE_FLAGS)

#CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS) -Wall
CFLAGS= -O0 -g $(INCLUDE_DIRS) $(CDEFS)
LIBS=

HFILES=
CFILES= bitrate.c eparse.c gopidx.c parse.c pids.c timeapi.c

PRODUCT= esparse pids bitrate gopidx libts.a

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	${PRODUCT}

bitrate: bitrate.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o ${LIB_DIRS} ${LIBS}

pids: pids.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o ${LIB_DIRS} ${LIBS}

gopidx: gopidx.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o ${LIB_DIRS} ${LIBS}

esparse: esparse.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $@.o ${LIB_DIRS} ${LIBS}

libts.a: timeapi.o
	ar -r $@ timeapi.o; ranlib $@

depend:

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f *.o *.exe *.a *.d ${PRODUCT}

distclean:
	-rm -f *.o *.exe *.a *.d ${PRODUCT}
