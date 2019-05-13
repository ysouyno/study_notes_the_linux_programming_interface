include ../Makefile.inc

all: ${TLPI_LIB}

OBJECTS = $(patsubst %.c, %.o, $(widcard *.c))

${TLPI_LIB}: ${OBJECTS}
	${RM} ${TLPI_LIB}
	${AR} rs ${TLPI_LIB} *.o

error_functions.o: ename.c.inc

ename.c.inc:
	sh build_ename.sh > ename.c.inc
	echo 1>&2 "ename.c.inc built"

clean:
	${RM} *.o ename.c.inc ${TLPI_LIB}
