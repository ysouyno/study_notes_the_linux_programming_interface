include ../Makefile.inc

GEN_EXE = fileio_test_00 fileio_test_01 signals_test_00 signals_test_01 \
	signals_test_02 signals_test_03 execl_test_00 t_etext_edata_end \
	t_gnu_argc_argv t_getpwnam t_getspnam t_getid t_setuid

EXE = ${GEN_EXE}

all: ${EXE}

allgen: ${GEN_EXE}

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
