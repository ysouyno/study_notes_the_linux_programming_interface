include ../Makefile.inc

GEN_EXE = ouch intquit t_kill sig_sender sig_receiver nonreentrant \
	sigmask_longjmp sigmask_siglongjmp t_sigaltstack t_sigqueue \
	catch_rtsigs t_sigsuspend t_sigwaitinfo signalfd_sigval

LINUX_EXE =

EXE = ${GEN_EXE} ${LINUX_EXE}

all: ${EXE}

allgen: ${GEN_EXE}

#
# The following example programs make use of signal() (instead of the
# preferred sigaction()) to establish signal handlers.
# By default, signal() on Linux follows the BSD semantics, establishing
# a reliable signal handler. This is because _BSD_SOURCE is enabled
# bt default in <features.h>. However, in ../Makefile.inc, we define
# CFLAGS to include _XOPEN_SOURCE, and this has the effect of disabling
# _BSD_SOURCE. To ensure that we get the default (BSD) semantics
# we here explicitly define _BSD_SOURCE when compiling these programs.
#

ouch: ouch.c
	${CC} -D_BSD_SOURCE -o $@ ouch.c ${CFLAGS} ${LDLIBS}

intquit: intquit.c
	${CC} -D_BSD_SOURCE -o $@ intquit.c ${CFLAGS} ${LDLIBS}

sig_receiver: sig_receiver.c
	${CC} -D_BSD_SOURCE -o $@ sig_receiver.c ${CFLAGS} ${LDLIBS}

nonreentrant: nonreentrant.o
	${CC} -o $@ nonreentrant.o ${CFLAGS} ${LDLIBS} ${LINUX_LIBCRYPT}

sigmask_siglongjmp.o: sigmask_longjmp.c
	${CC} -o $@ -DUSE_SIGSETJMP -c sigmask_longjmp.c ${CFLAGS}

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
