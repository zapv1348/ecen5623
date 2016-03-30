INCLUDE_DIRS = 
LIB_DIRS = 

CDEFS=
CFLAGS= -O -g $(INCLUDE_DIRS) $(CDEFS) -DLINUX
LIBS=-lpthread -lrt

HFILES= heap_mq.c posix_mq.c
CFILES1= heap_test.c 
CFILES2= heap_test1.c 
CFILES3= posix_test.c 

SRCS1= ${HFILES} ${CFILES1}
SRCS2= ${HFILES} ${CFILES2}
SRCS3= ${HFILES} ${CFILES3}
OBJS1= ${CFILES1:.c=.o}
OBJS2= ${CFILES2:.c=.o}
OBJS3= ${CFILES3:.c=.o}

all: heap_test heap_test1 posix_test

clean:
	-rm -f *.o *.d *.exe heap_test heap_test1 posix_test

heap_test: heap_test.o heap_mq.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS1) $(LIBS)

heap_test1: heap_test1.o heap_mq.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS2) $(LIBS)

posix_test: posix_test.o posix_mq.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o $@ $(OBJS3) $(LIBS)

depend:

.c.o:
	$(CC) -MD $(CFLAGS) -c $<
