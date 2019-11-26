include ../Makefile.inc

GEN_EXE = pmsg_receive

LINUX_EXE =

EXE = ${GEN_EXE} ${LINUX_EXE}

all: ${EXE}

allgen: ${GEN_EXE}

# All of the programs in this directory need the
# realtime library, librt.
LDLIBS = ${IMPL_LDLIBS} ${LINUX_LIBRT}

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
