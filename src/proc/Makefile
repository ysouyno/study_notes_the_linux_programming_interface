include ../Makefile.inc

GEN_EXE = display_env modify_env longjmp setjmp_vars

EXE = ${GEN_EXE}

all: ${EXE}

allgen: ${GEN_EXE}

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
