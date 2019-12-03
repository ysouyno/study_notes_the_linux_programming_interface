DIRS = lib \
	fileio \
	test \
	getopt \
	exercises \
	proc \
	memalloc \
	users_groups \
	proccred \
	time \
	signals \
	procexec \
	threads \
	pipes \
	svmsg \
	svsem \
	svshm \
	mmap \
	pmsg \
	sockets \
	altio

BUILD_DIRS = ${DIRS}

all:
	@ echo ${BUILD_DIRS}
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE}); \
		if test $$? -ne 0; then break; fi; done

clean:
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE} clean); done
