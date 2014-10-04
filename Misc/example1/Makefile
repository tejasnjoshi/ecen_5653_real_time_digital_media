NCLUDE_DIRS = 
LIB_DIRS = 

CDEFS= -DPMU_ANALYSIS
CFLAGS= -O3 -g $(INCLUDE_DIRS) $(CDEFS) 
LIBS= -lrt

PRODUCT=perfmon

HFILES=
CFILES= perfmon.c

SRCS= ${HFILES} ${CFILES}
OBJS= ${CFILES:.c=.o}

all:	${PRODUCT}

clean:
	-rm -f *.o *.NEW *~ *.d
	-rm -f ${PRODUCT} ${GARBAGE}

${PRODUCT}:	${OBJS}
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

depend:

.c.o:
	$(CC) -MD $(CFLAGS) -c $<
