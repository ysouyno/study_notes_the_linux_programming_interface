include ../Makefile.inc

GEN_EXE = simple_thread thread_incr thread_incr_mutex prod_no_condvar \
	prod_condvar thread_multijoin strerror_test strerror_test_tsd \
	thread_cancel thread_cleanup

LINUX_EXE = strerror_test_tls

EXE = ${GEN_EXE} ${LINUX_EXE}

CFLAGS = ${IMPL_CFLAGS} ${IMPL_THREAD_FLAGS}
LDLIBS = ${IMPL_LDLIBS} ${IMPL_THREAD_FLAGS}

all: ${EXE}

allgen: ${GEN_EXE}

strerror_test: strerror_test.o strerror.o
	${CC} -o $@ strerror_test.o strerror.o \
		${CFLAGS} ${LDLIBS}

strerror_test_tsd: strerror_test.o strerror_tsd.o
	${CC} -o $@ strerror_test.o strerror_tsd.o \
		${CFLAGS} ${LDLIBS}

strerror_test_tls: strerror_test.o strerror_tls.o
	${CC} -o $@ strerror_test.o strerror_tls.o \
		${CFLAGS} ${LDLIBS}

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
