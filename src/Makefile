uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')
uname_R := $(shell sh -c 'uname -r 2>/dev/null || echo not')

ifeq ($(uname_S),SunOS)
	LIBSUFFIX=so
else
	LIBSUFFIX=a
endif

PROGRAM=qnd
OBJS=qnd.o client.o server.o callback.o handler.o sds.o
CFLAGS=-g -Wall -pedantic -O2 -std=c99 -I ../deps/target/include
LIBS=../deps/target/lib/libev.$(LIBSUFFIX) ../deps/target/lib/libstatgrab.$(LIBSUFFIX)

ifeq ($(uname_S),Linux)
	CFLAGS+= -D_XOPEN_SOURCE
	LIBS+= -lpthread -lm
endif
ifeq ($(uname_S),FreeBSD)
	LIBS+= -ldevstat -lpthread -lm
endif
ifeq ($(uname_S),Darwin)
	LIBS+= -framework CoreFoundation -framework IOKit
endif
ifeq ($(uname_S),SunOS)
	CFLAGS+= -D__EXTENSIONS__
	LIBS+= -lnsl -lsocket -lrt -lkstat -ldevinfo -pthreads
	ifeq ($(uname_R),5.8)
		CFLAGS+= -D_XOPEN_SOURCE -D_XPG4_2
		LIBS+= -Wl,-rpath -Wl,../deps/target/lib
	else
		LIBS+= -R ../deps/target/lib
	endif
endif

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $@

clean:
	@rm -rf $(OBJS) $(PROGRAM)
