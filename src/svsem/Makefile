include ../Makefile.inc

GEN_EXE = svsem_demo svsem_mon svsem_setall svsem_good_init svsem_op \
	svsem_create svsem_rm

LINUX_EXE =

EXE = ${GEN_EXE} ${LINUX_EXE}

all: ${EXE}

allgen: ${GEN_EXE}

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
