uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

PROGRAM=qnd
OBJS=qnd.o client.o server.o callback.o handler.o sds.o
LIBS=../deps/target/lib/libev.a ../deps/target/lib/libstatgrab.a
CFLAGS=-g -Wall -Werror -O0 -std=c99 -I ../deps/target/include

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
	LIBS+= -lnsl -lsocket -lev -lstatgrab -L ../deps/target/lib
endif

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $@

clean:
	@rm -rf $(OBJS) $(PROGRAM)
