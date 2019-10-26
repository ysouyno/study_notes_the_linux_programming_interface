include ../Makefile.inc

GEN_EXE = 04_01_tee 20_01_sig_receiver 20_02 20_03 22_01 22_02 22_03 \
	24_02 24_03 24_05 26_01 26_02 26_03 26_04 29_01

EXE = ${GEN_EXE}

CFLAGS = ${IMPL_CFLAGS} ${IMPL_THREAD_FLAGS}
LDLIBS = ${IMPL_CFLAGS} ${IMPL_THREAD_FLAGS}

all: ${EXE}

allgen: ${GEN_EXE}

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB} # True as a rough approximation
