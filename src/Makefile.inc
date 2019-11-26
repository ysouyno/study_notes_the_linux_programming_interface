# Makefile.inc - common definitions used by all makefiles

TLPI_DIR = ..
TLPI_LIB = ${TLPI_DIR}/libtlpi.a
TLPI_INCL_DIR = ${TLPI_DIR}/lib

LINUX_LIBCRYPT = -lcrypt
LINUX_LIBRT = -lrt

IMPL_CFLAGS = -std=c99 -D_XOPEN_SOURCE=600 \
            -D_DEFAULT_SOURCE \
            -g -I${TLPI_INCL_DIR} \
            -pedantic \
            -Wall \
            -W \
            -Wmissing-prototypes \
            -Wno-sign-compare \
            -Wno-unused-parameter

CFLAGS = ${IMPL_CFLAGS}

IMPL_THREAD_FLAGS = -pthread

IMPL_LDLIBS = ${TLPI_LIB}

LDLIBS = ${IMPL_LDLIBS}

RM = rm -f