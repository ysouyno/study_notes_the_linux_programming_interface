include ../Makefile.inc

GEN_EXE = simple_thread thread_incr thread_incr_mutex prod_no_condvar

LINUX_EXE =

EXE = ${GEN_EXE} ${LINUX_EXE}

CFLAGS = ${IMPL_CFLAGS} ${IMPL_THREAD_FLAGS}
LDLIBS = ${IMPL_CFLAGS} ${IMPL_THREAD_FLAGS}

all: ${EXE}

allgen: ${GEN_EXE}

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
