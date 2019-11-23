include ../Makefile.inc

GEN_EXE = svshm_xfr_writer svshm_xfr_reader

LINUX_EXE =

EXE = ${GEN_EXE} ${LINUX_EXE}

all: ${EXE}

allgen: ${GEN_EXE}

svshm_xfr_writer.o svshm_xfr_reader.o: svshm_xfr.h

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
