include ../Makefile.inc

GEN_EXE = simple_pipe pipe_sync pipe_ls_wc popen_glob fifo_seqnum_server \
	fifo_seqnum_client

LINUX_EXE =

EXE = ${GEN_EXE} ${LINUX_EXE}

all: ${EXE}

allgen: ${GEN_EXE}

fifo_seqnum_server.o fifo_seqnum_client.o: fifo_seqnum.h

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
