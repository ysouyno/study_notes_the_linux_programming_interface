include ../Makefile.inc

GEN_EXE = svmsg_create svmsg_send svmsg_receive svmsg_rm svmsg_chqbytes \
	svmsg_file_server svmsg_file_client

LINUX_EXE = svmsg_ls

EXE = ${GEN_EXE} ${LINUX_EXE}

all: ${EXE}

allgen: ${GEN_EXE}

clean:
	${RM} ${EXE} *.o

svmsg_file_server.o svmsg_file_client.o: svmsg_file.h

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
