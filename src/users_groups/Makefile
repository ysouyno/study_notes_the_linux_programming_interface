include ../Makefile.inc

GEN_EXE =

LINUX_EXE = check_password

EXE = ${GEN_EXE} ${LINUX_EXE}

all: ${EXE}

allgen: ${GEN_EXE}

clean:
	${RM} ${EXE} *.o

check_password: check_password.o
	${CC} -o $@ check_password.o ${LDFLAGS} ${LDLIBS} ${LINUX_LIBCRYPT}

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
