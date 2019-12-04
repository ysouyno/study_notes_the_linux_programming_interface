include ../Makefile.inc

GEN_EXE = t_select poll_pipes demo_sigio epoll_input self_pipe

LINUX_EXE =

EXE = ${GEN_EXE} ${LINUX_EXE}

all: ${EXE}

allgen: ${GEN_EXE}

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
