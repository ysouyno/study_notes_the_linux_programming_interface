<!-- markdown-toc start - Don't edit this section. Run M-x markdown-toc-refresh-toc -->
**Table of Contents**

- [<2019-05-15 周三>](#2019-05-15-周三)
    - [《The Linux Programming Interface》读书笔记（一）](#the-linux-programming-interface读书笔记一)
        - [关于根目录的`Makefile`的理解](#关于根目录的makefile的理解)
- [<2019-05-17 周五>](#2019-05-17-周五)
    - [《The Linux Programming Interface》读书笔记（二）](#the-linux-programming-interface读书笔记二)
        - [关于根目录的`Makefile.inc`的理解](#关于根目录的makefileinc的理解)
        - [关于`lib`目录下的`Makefile`的理解](#关于lib目录下的makefile的理解)
        - [关于`fileio`目录下的`Makefile`的理解](#关于fileio目录下的makefile的理解)
        - [如何确保使用标准输入（文件描述符`0`）打开一文件](#如何确保使用标准输入文件描述符0打开一文件)
- [<2019-05-20 周一>](#2019-05-20-周一)
    - [《The Linux Programming Interface》读书笔记（三）](#the-linux-programming-interface读书笔记三)
        - [如何用`grep`在文件中查找字符串](#如何用grep在文件中查找字符串)
- [<2019-06-03 周一>](#2019-06-03-周一)
    - [《The Linux Programming Interface》读书笔记（四）](#the-linux-programming-interface读书笔记四)
        - [关于`O_EXCL`的用途](#关于oexcl的用途)
- [<2019-06-20 周四>](#2019-06-20-周四)
    - [《The Linux Programming Interface》读书笔记（五）](#the-linux-programming-interface读书笔记五)
        - [关于`putenv`和`setenv`的使用](#关于putenv和setenv的使用)
        - [关于`setjmp`和`longjmp`的使用](#关于setjmp和longjmp的使用)
        - [关于`setjmp`和`longjmp`的编译器优化问题](#关于setjmp和longjmp的编译器优化问题)
- [<2019-06-21 周五>](#2019-06-21-周五)
    - [《The Linux Programming Interface》读书笔记（六）](#the-linux-programming-interface读书笔记六)
        - [关于`brk`、`sbrk`和`program break`的理解](#关于brksbrk和program-break的理解)
        - [关于`alloca`的使用](#关于alloca的使用)
- [<2019-06-22 周六>](#2019-06-22-周六)
    - [《The Linux Programming Interface》读书笔记（七）](#the-linux-programming-interface读书笔记七)
        - [Set-User-ID and Set-Group-ID Programs](#set-user-id-and-set-group-id-programs)

<!-- markdown-toc end -->

# <2019-05-15 周三>

## 《The Linux Programming Interface》读书笔记（一）

### 关于根目录的`Makefile`的理解

纠结了好几天，因为下载到的`TLPI`的书中代码有自己定义的头文件和很多`Makefile`文件，让我有点望而却步，还真花了近三天的时间去配置`TLPI`的读书环境，这样一来对`Makefile`的理解加深了许多。

所以这里有必要对`Makefile`文件做一些解释，下面是代码所在目录的根目录中的`Makefile`文件内容，后续会随着读书进度对该文件内容进行增加，直到和书中代码提供的`Makefile`内容一样，目前内容如下：

```
DIRS = lib \
	fileio

BUILD_DIRS = ${DIRS}

all:
	@ echo ${BUILD_DIRS}
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE}); \
		if test $$? -ne 0; then break; fi; done

clean:
	@ for dir in ${BUILD_DIRS}; do (cd $${dir}; ${MAKE} clean); done
```

`lib`目录位于`DIRS`的第一个，表示`lib`目录中的文件将最先被编译；`@`的意思是关闭回显，如果去掉，在运行`make`时将会有如下显示：

```
cho lib fileio
lib fileio
for dir in lib fileio; do (cd ${dir}; make); \
	if test $? -ne 0; then break; fi; done
```

即将`Makefile`中的代码内容打印出来了；`$${dir}`前面的`$$`表示`dir`是一个局部变量，在`shell`编程中局部变量用`$$`修饰；`${MAKE}`是`make`预定义的宏，如果要查看这些宏，请使用`make -p`命令，比如：

```
% make -p | grep MAKE
make: *** No targets specified and no makefile found.  Stop.
MAKE_VERSION := 4.2.1
MAKEFILE_LIST := 
GNUMAKEFLAGS := 
MAKELEVEL := 0
MAKE_HOST := x86_64-pc-linux-gnu
MAKE = $(MAKE_COMMAND)
MAKE_COMMAND := make
MAKEFLAGS = p
MAKEFILES := 
MAKEINFO = makeinfo
MAKE_TERMERR := /dev/pts/2
	$(MAKEINFO) $(MAKEINFO_FLAGS) $< -o $@
	$(MAKEINFO) $(MAKEINFO_FLAGS) $< -o $@
	$(MAKEINFO) $(MAKEINFO_FLAGS) $< -o $@
	$(MAKEINFO) $(MAKEINFO_FLAGS) $< -o $@
	$(MAKEINFO) $(MAKEINFO_FLAGS) $< -o $@
	$(MAKEINFO) $(MAKEINFO_FLAGS) $< -o $@
```

这里的`$$?`是什么意思呢？我没有找到关于`$`的解释，但是根据自测可以得知`$`类似于转义`\`，比如我去掉前面的`@`后从终端打印出来的`shell`代码可以看出，如下：

```
for dir in lib fileio; do (cd ${dir}; make); \
	if test  -ne 0; then break; fi; done
```
```
# 正确输出
for dir in lib fileio; do (cd ${dir}; make); \
	if test $? -ne 0; then break; fi; done
```

`test`后面不是`$?`而是空格，所以我理解是`$$?`执行时变成了`$?`来判断命令的退出状态，`0`表示没有错误，其他任何值表明有错误。

# <2019-05-17 周五>

## 《The Linux Programming Interface》读书笔记（二）

### 关于根目录的`Makefile.inc`的理解

目前`Makefile.inc`的内容如下：

```
# Makefile.inc - common definitions used by all makefiles

TLPI_DIR = ..
TLPI_LIB = ${TLPI_DIR}/libtlpi.a
TLPI_INCL_DIR = ${TLPI_DIR}/lib

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

IMPL_LDLIBS = ${TLPI_LIB}

LDLIBS = ${IMPL_LDLIBS}

RM = rm -f
```

很好理解，但是需要提出一个问题，在运行`make`会有如下的输出：

```
cc -std=c99 -D_XOPEN_SOURCE=600 -D_DEFAULT_SOURCE -g -I../lib -pedantic -Wall -W -Wmissing-prototypes -Wno-sign-compare -Wno-unused-parameter   -c -o get_num.o get_num.c
```

上面输出中的`-c -o get_num.o get_num.c`这部分内容为什么会自动被加入`cc`的选项里？目前我还没有找到答案。

### 关于`lib`目录下的`Makefile`的理解

目前`Makefile`的内容如下：

```
include ../Makefile.inc

all: ${TLPI_LIB}

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))

${TLPI_LIB}: ${OBJECTS}
	${RM} ${TLPI_LIB}
	${AR} rs ${TLPI_LIB} *.o

error_functions.o: ename.c.inc

ename.c.inc:
	sh build_ename.sh > ename.c.inc
	echo 1>&2 "ename.c.inc built"

clean:
	${RM} *.o ename.c.inc ${TLPI_LIB}
```

代码中的`patsubst`的作用是将`%.c`改为`%.o`，`wildcard *.c`是搜索当前目录下所有以`.c`结尾的文件，生成一个以空格间隔的文件名列表。

### 关于`fileio`目录下的`Makefile`的理解

目前`Makefile`的内容如下：

```
include ../Makefile.inc

GEN_EXE = copy

LINUX_EXE = large_file

EXE = ${GEN_EXE} ${LINUX_EXE}

all: ${EXE}

allgen: ${GEN_EXE}

clean:
	${RM} ${EXE} *.o

showall:
	@ echo ${EXE}

${EXE}: ${TLPI_LIB}
```

代码中`LINUX_EXE = large_file`这是啥意思？没有找到`large_file`的解释。

### 如何确保使用标准输入（文件描述符`0`）打开一文件

如果调用`open()`成功，必须保证其返回值为进程未用文件描述符中数值最小者，补全书中代码如下：

```
// fileio_test_00.c
#include <fcntl.h>
#include <tlpi_hdr.h>

int main(int argc, char *argv[])
{
  int fd;

  if (close(STDIN_FILENO) == -1) {
    errExit("close");
  }

  fd = open("/home/ysouyno/temp/a.txt", O_RDONLY);
  if (fd == -1) {
    errExit("open");
  }

  printf("fd: %d\n", fd);

  if (close(fd) == -1) {
    errExit("close");
  }

  return 0;
}
```

由于文件描述符`0`未用，所以`open()`调用势必使用此描述符打开文件。测试输出结果：`fd: 0`。

# <2019-05-20 周一>

## 《The Linux Programming Interface》读书笔记（三）

### 如何用`grep`在文件中查找字符串

在附件B中看到了这个用法，比如`grep -- -p t_getopt.c`表示在`t_getopt.c`文件中查找字符串`-p`，必须要使用`--`，不用说，否则`-p`会被当做`grep`的参数处理而无法查找字符串`-p`，如下：

```
% grep -- -p t_getopt.c
  fprintf(stderr, "Usage :%s [-p arg] [-x]\n", prog_name);
    printf("-p was specified with the value \"%s\"\n", pstr);
```

可以直接在文件中查找字符串，如下：

```
% grep stderr t_getopt.c
    fprintf(stderr, "%s (-%c)\n", msg, printable(opt));
  fprintf(stderr, "Usage :%s [-p arg] [-x]\n", prog_name);
```

# <2019-06-03 周一>

## 《The Linux Programming Interface》读书笔记（四）

### 关于`O_EXCL`的用途

书中代码`bad_exclusive_open.c`展示了两个程序都声称自己独家创建了一个文件，代码如下：

```
#include <sys/stat.h>
#include <fcntl.h>
#include "tlpi_hdr.h"

int main(int argc, char *argv[])
{
  int fd;

  if (argc < 2 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s file\n", argv[0]);
  }

  fd = open(argv[1], O_WRONLY); // Open 1: check if file exists
  if (fd != -1) {               // Open succeeded
    printf("[PID %ld] File \"%s\" already exists\n",
           (long)getpid(), argv[1]);
    close(fd);
  }
  else {
    if (errno != ENOENT) {      // Failed for unexpected reason
      errExit("open");
    }
    else {
      printf("[PID %ld] File \"%s\" doesn't exist yet\n",
             (long)getpid(), argv[1]);
      if (argc > 2) {           // Delay between check and create
        sleep(5);               // Suspend execution for 5 seconds
        printf("[PID %ld] Done sleeping\n", (long)getpid());
      }

      fd = open(argv[1], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
      if (fd == -1) {
        errExit("open");
      }

      printf("[PID %ld] Create file \"%s\" exclusively\n",
             (long)getpid(), argv[1]); // MAY NOT BE TRUE!
    }
  }

  exit(EXIT_SUCCESS);
}
```

用到的测试命令为：

```
./bad_exclusive_open tfile sleep &
./bad_exclusive_open tfile
```

我的机器输出如下：

```
-> % ./fileio/bad_exclusive_open tfile sleep &
[1] 6422
[PID 6422] File "tfile" doesn't exist yet

-> % ./fileio/bad_exclusive_open tfile
[PID 6431] File "tfile" doesn't exist yet
[PID 6431] Create file "tfile" exclusively

-> % [PID 6422] Done sleeping
[PID 6422] Create file "tfile" exclusively

[1]  + 6422 done       ./fileio/bad_exclusive_open tfile sleep
```

可添加`O_EXCL`参数来解决这个问题，将`fd = open(argv[1], O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);`修改为`fd = open(argv[1], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);`，这样代码输出如下：

```
-> % ./fileio/bad_exclusive_open tfile sleep &
[1] 6508
[PID 6508] File "tfile" doesn't exist yet

-> % ./fileio/bad_exclusive_open tfile
[PID 6517] File "tfile" doesn't exist yet
[PID 6517] Create file "tfile" exclusively

-> % [PID 6508] Done sleeping
ERROR [EEXIST File exists] open

[1]  + 6508 exit 1     ./fileio/bad_exclusive_open tfile sleep
```

# <2019-06-20 周四>

## 《The Linux Programming Interface》读书笔记（五）

### 关于`putenv`和`setenv`的使用

每次拿起这本书看看都会困，讲得都是某个函数怎么使用，拿来当参考书还是不错的，比`man`手册页讲得详细。

```
#include <stdlib.h>
int putenv(char *string);
Returns 0 on success, or nonzero on error
```

```
#include <stdlib.h>
int setenv(const char *name, const char *value, int overwrite);
Returns 0 on success, or 1 on error
```

`putenv`和`setenv`的函数的定义如上，它们的区别除了参数表达的意思外，我想说的是`putenv`传入的指针，后续针对参数`string`的修改会影响到环境变量，而`setenv`不会。`setenv`会分配内存并将指针指向的字符串内容拷贝过去。

所以在使用的时候要小心，我好像在`man`手册页上没看到这些内容，所以多看看这本书还是有好处的。

贴上书中的代码如下：

```
// modify_env.c
#define _GNU_SOURCE // To get various declaration from <stdlib.h>
#include <stdlib.h>
#include "tlpi_hdr.h"

extern char **environ;

int main(int argc, char *argv[])
{
  int j;
  char **ep;

  clearenv(); // Erase entire environment

  for (j = 1; j < argc; ++j) {
    if (putenv(argv[j]) != 0) {
      errExit("putenv: %s", argv[j]);
    }
  }

  if (setenv("GREET", "Hello world", 0) == -1) {
    errExit("setenv");
  }

  unsetenv("BYE");

  for (ep = environ; *ep != NULL; ++ep) {
    puts(*ep);
  }

  exit(EXIT_SUCCESS);
}
```
```
% ./modify_env "GREET=ysouyno sun" SHELL=/bin/bash BYE=bye
GREET=ysouyno sun
SHELL=/bin/bash

./modify_env SHELL=/bin/bash BYE=bye
SHELL=/bin/bash
GREET=Hello world
```

### 关于`setjmp`和`longjmp`的使用

原来`linux`的`C`语言还有这种骚操作，类似于`goto`，但是`goto`只能在函数内部进行跳转，而`setjmp`和`longjmp`这两个函数能用于函数间跳转，`setjmp`设置跳转的目的地，`longjmp`跳转到此。正如书上所说，要特别注意：__如果`setjmp`的函数已经调用返回了，再执行`longjmp`的话会产生错误，因为展开堆栈会有问题__。

见代码如下：

```
// longjmp.c
#include <setjmp.h>
#include "tlpi_hdr.h"

static jmp_buf env;

static void f2()
{
  longjmp(env, 2);
}

static void f1(int argc)
{
  if (argc == 1) {
    longjmp(env, 1);
  }

  f2();
}

int main(int argc, char *argv[])
{
  switch (setjmp(env)) {
  case 0: // This is the return after the initial setjmp()
    printf("Calling f1() after initial setjmp()\n");
    f1(argc);
    break;
  case 1:
    printf("We jumped back from f1()\n");
    break;
  case 2:
    printf("We jumped back from f2()\n");
    break;
  }

  exit(EXIT_SUCCESS);
}
```
```
% ./longjmp
Calling f1() after initial setjmp()
We jumped back from f1()

% ./longjmp x
Calling f1() after initial setjmp()
We jumped back from f2()
```

### 关于`setjmp`和`longjmp`的编译器优化问题

这两个函数还是尽量不要使用，因为编译器优化会带来的很大的问题，如果要使用这两个函数，内部涉及跳转的变量请使用`volatile`关键字来修饰，因为这样可以防止编译器优化。如下面的代码所示：

```
// setjmp_var.c
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>

static jmp_buf env;

static void doJump(int nvar, int rvar, int vvar)
{
  printf("Inside doJump(): nvar = %d rvar = %d vvar = %d\n",
         nvar, rvar, vvar);
  longjmp(env, 1);
}

int main(int argc, char *argv[])
{
  int nvar;
  register int rvar;      // Allocated in register if possible
  volatile int vvar;

  nvar = 111;
  rvar = 222;
  vvar = 333;

  if (setjmp(env) == 0) { // Code executed after setjmp()
    nvar = 777;
    rvar = 888;
    vvar = 999;
    doJump(nvar, rvar, vvar);
  }
  else {                  // Code executed after longjmp()
    printf("After longjmp(): nvar = %d rvar = %d vvar = %d\n",
           nvar, rvar, vvar);
  }

  exit(EXIT_SUCCESS);
}
```
```
% cc -o setjmp_vars setjmp_vars.c

% ./setjmp_vars
Inside doJump(): nvar = 777 rvar = 888 vvar = 999
After longjmp(): nvar = 777 rvar = 222 vvar = 999
```
```
% cc -O -o setjmp_vars setjmp_vars.c

% ./setjmp_vars
Inside doJump(): nvar = 777 rvar = 888 vvar = 999
After longjmp(): nvar = 111 rvar = 222 vvar = 999
```

从上面代码可以看到只有`volatile`修饰的`vvar`变量没有变化，因为`volatile`明确指明不需要编译器优化此变量，才使得`vvar`是我们期望的值。

书上说可以通过添加`-Wextra`产生有用的编译器警告信息，但是我的显示却是这样的，说明这本书有点久远了。

```
% cc -Wall -Wextra -O -o setjmp_vars setjmp_vars.c
setjmp_vars.c: In function 'main':
setjmp_vars.c:14:14: warning: unused parameter 'argc' [-Wunused-parameter]
 int main(int argc, char *argv[])
          ~~~~^~~~
setjmp_vars.c:14:26: warning: unused parameter 'argv' [-Wunused-parameter]
 int main(int argc, char *argv[])
                    ~~~~~~^~~~~~
```

可以使用`make`产生了有用的警告信息，如下：

```
setjmp_vars.c: In function 'main':
setjmp_vars.c:17:16: warning: variable 'rvar' might be clobbered by 'longjmp' or 'vfork' [-Wclobbered]
   register int rvar;      // Allocated in register if possible
                ^~~~
```

# <2019-06-21 周五>

## 《The Linux Programming Interface》读书笔记（六）

### 关于`brk`、`sbrk`和`program break`的理解

这篇文章我觉得讲得不错“[虚拟内存探究 -- 第四篇: malloc, heap & the program break](https://cloud.tencent.com/developer/article/1198231)”。

当程序刚开始初始化的时候`program break`是指向`the uninitialized data segment`的结束处，如果程序中有调用`malloc`从堆上分配内存，则对于刚初始化的程序而言必须增加`program break`的位置，这样`malloc`就可以在`the end of the uninitialized data segment`和新的`program break`之间申请内存了，这部分操作可能是被放在了`malloc`函数内部实现的，通过调用`brk`和`sbrk`系统调用来实现的。

> Resizing the heap (i.e., allocating or deallocating memory) is actually as simple as telling the kernel to adjust its idea of where the process’s program break is.

引用文章中的图片：

![](files/program_break_0.png)

按照文章的介绍我使用了`strace`来跟踪系统调用情况，有如下输出：

```
write(1, "BEFORE MALLOC\n", 14BEFORE MALLOC
)         = 14
brk(NULL)                               = 0x55fee314b000
brk(0x55fee316c000)                     = 0x55fee316c000
write(1, "AFTER MALLOC\n", 13AFTER MALLOC
)          = 13
```

可以结合书中的代码加以理解，如下代码展示了当程序内存释放后`program break`发生了什么，如下：

```
// free_and_sbrk.c
#include "tlpi_hdr.h"

#define MAX_ALLOCS 1000000

int main(int argc, char *argv[])
{
  char *ptr[MAX_ALLOCS];
  int free_step, free_min, free_max, block_size, num_allocs, j;

  if (argc < 3 || strcmp(argv[1], "--help") == 0) {
    usageErr("%s num-allocs block-size [step [min [max]]]\n", argv[0]);
  }

  num_allocs = getInt(argv[1], GN_GT_0, "num-allocs");
  if (num_allocs > MAX_ALLOCS) {
    cmdLineErr("num-allocs > %d\n", MAX_ALLOCS);
  }

  block_size = getInt(argv[2], GN_GT_0 | GN_ANY_BASE, "block-size");

  free_step = (argc > 3) ? getInt(argv[3], GN_GT_0, "step") : 1;
  free_min  = (argc > 4) ? getInt(argv[4], GN_GT_0, "min") : 1;
  free_max  = (argc > 5) ? getInt(argv[5], GN_GT_0, "max") : num_allocs;

  if (free_max > num_allocs) {
    cmdLineErr("free-max > num-allocs\n");
  }

  printf("Initial program break:           %10p\n", sbrk(0));

  printf("Allocating %d*%d bytes\n", num_allocs, block_size);

  for (j = 0; j < num_allocs; ++j) {
    ptr[j] = malloc(block_size);
    if (ptr[j] == NULL) {
      errExit("malloc");
    }
  }

  printf("Program break is now:            %10p\n", sbrk(0));

  printf("Freeing blocks from %d to %d in steps of %d\n",
         free_min, free_max, free_step);

  for (j = free_min - 1; j < free_max; j += free_step) {
    free(ptr[j]);
  }

  printf("After free(), program break is : %10p\n", sbrk(0));

  exit(EXIT_SUCCESS);
}
```

从下面两种输出来看`program break`并没有改变

```
% ./free_and_sbrk 1000 10240 2
Initial program break:           0x55eaf7dc8000
Allocating 1000*10240 bytes
Program break is now:            0x55eaf87b1000
Freeing blocks from 1 to 1000 in steps of 2
After free(), program break is : 0x55eaf87b1000

% ./free_and_sbrk 1000 10240 1 1 999
Initial program break:           0x555ec439a000
Allocating 1000*10240 bytes
Program break is now:            0x555ec4d83000
Freeing blocks from 1 to 999 in steps of 1
After free(), program break is : 0x555ec4d83000
```

但是下面这个输出，将申请的`1000`个内存的最后`500`个全部释放了，`program break`也跟着变小了，说明系统回收了部分，整个程序的堆变小了又，如下输出：

```
% ./free_and_sbrk 1000 10240 1 500 1000
Initial program break:           0x55e29c454000
Allocating 1000*10240 bytes
Program break is now:            0x55e29ce3d000
Freeing blocks from 500 to 1000 in steps of 1
After free(), program break is : 0x55e29c956000
```

### 关于`alloca`的使用

原来还有可以在栈上分配内存的函数，这就是`alloca`，比较好奇的是要在什么情况下可以使用它呢？看完原文这段话就会理解了：

> Using `alloca()` can be especially useful if we employ `longjmp()` (Section 6.8) or `siglongjmp()` (Section 21.2.1) to perform a nonlocal goto from a signal handler. In this case, it is difficult or even impossible to avoid memory leaks if we allocated memory in the jumped-over functions using `malloc()`. By contrast, `alloca()` avoids this problem completely, since, as the stack is unwound by these calls, the allocated memory is automatically freed.

同时`alloca`使用时必须注意不能做为函数的参数，在我的理解就是函数参数是分配在栈上的，此时函数还没有调用，栈空间还不知道在哪里呢？您将`alloca`做为参数传入，那么`alloca`的内存要分配哪里？正确的使用如下：

```
void *y;
y = alloca(size);
func(x, y, z);
```

# <2019-06-22 周六>

## 《The Linux Programming Interface》读书笔记（七）

### Set-User-ID and Set-Group-ID Programs

这本书原来不打算再看了，一看就困，感觉好浅。其实还是有必要看的，我没有我自己认为的那样懂`linux`系统，比如这个标题我就是完全陌生，感觉好恐怖！

原版英文看得不太懂，拿代码中的`check_password.c`这个程序来举例，正如书上的一样，如果我以我的登录用户名`ysouyno`直接执行的话，会显示如下错误：

```
% ./check_password
Username: ysouyno
ERROR: no permisson to read shadow password file
```

那么这里就需要使用`chmod u+s`来将`check_password`设置为`Set-User-ID`程序了，如下的一系列命令的输出：

```
% su
Password:

# chown root check_password

# ls -l
total 60
-rw-r--r-- 1 ysouyno ysouyno   302 Jun 22 08:23 Makefile
-rwxr-xr-x 1 root    ysouyno 35232 Jun 22 10:35 check_password
-rw-r--r-- 1 ysouyno ysouyno  1815 Jun 22 08:23 check_password.c
-rw-r--r-- 1 ysouyno ysouyno 15360 Jun 22 10:35 check_password.o

# chmod u+s check_password
# ls -l
total 60
-rw-r--r-- 1 ysouyno ysouyno   302 Jun 22 08:23 Makefile
-rwsr-xr-x 1 root    ysouyno 35232 Jun 22 10:35 check_password
-rw-r--r-- 1 ysouyno ysouyno  1815 Jun 22 08:23 check_password.c
-rw-r--r-- 1 ysouyno ysouyno 15360 Jun 22 10:35 check_password.o

# exit
exit

% ./check_password
Username: ysouyno
Password: 
Successfully authenticated: UID=1000
```

注意到此时`check_password`的权限从`-rwxr-xr-x`变成了`-rwsr-xr-x`，同时必须注意这里首先使用`chown`将`check_password`的所有者变成`root`，因为当`Set-User-ID`程序`check_password`执行时程序就具有文件所有者的权限，即`root`权限。
