include ../Makefile.inc

GEN_EXE = us_xfr_sv us_xfr_cl ud_ucase_sv ud_ucase_cl

LINUX_EXE =

EXE = ${GEN_EXE} ${LINUX_EXE}

all: ${EXE}

allgen: ${GEN_EXE}

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
