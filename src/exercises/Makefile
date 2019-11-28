include ../Makefile.inc

GEN_EXE = 04_01_tee 20_01_sig_receiver 20_02 20_03 22_01 22_02 22_03 \
	24_02 24_03 24_05 26_01 26_02 26_03 26_04 29_01 31_01 44_01 \
	44_02_test_0 44_02_test_1 44_03 44_04 45_01 46_02_server 46_02_client \
	46_04_server 46_04_client 46_05_client 47_02 47_07 \
	48_01_xfr_writer 48_01_xfr_reader 48_04 48_06 49_01 \
	49_02_xfr_writer 49_02_xfr_reader 49_03 52_01 52_02_seqnum_server \
	52_02_seqnum_client 52_03_file_server 52_03_file_client 52_06 \
	53_01

EXE = ${GEN_EXE}

CFLAGS = ${IMPL_CFLAGS} ${IMPL_THREAD_FLAGS}
LDLIBS = ${IMPL_CFLAGS} ${IMPL_THREAD_FLAGS} ${LINUX_LIBRT}

all: ${EXE}

allgen: ${GEN_EXE}

44_02_test_0: 44_02.o

44_02_test_1: 44_02.o

44_03.o: fifo_seqnum.h

44_04.o: fifo_seqnum.h

48_01_xfr_writer.o 48_01_xfr_reader.o: svshm_xfr.h

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB} # True as a rough approximation
