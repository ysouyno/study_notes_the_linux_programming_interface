include ../Makefile.inc

GEN_EXE = svmsg_create svmsg_send svmsg_receive svmsg_rm svmsg_chqbytes \
	svmsg_ls

LINUX_EXE =

EXE = ${GEN_EXE} ${LINUX_EXE}

all: ${EXE}

allgen: ${GEN_EXE}

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
