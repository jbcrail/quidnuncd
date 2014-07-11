CFLAGS  = -I ../deps/target/include
LDFLAGS = -framework CoreFoundation -framework IOKit

LIBS = ../deps/target/lib/libev.a ../deps/target/lib/libstatgrab.a
OBJS = qnd.o context.o cmd.o callback.o

PROGRAM = qnd

all: $(PROGRAM)

$(PROGRAM): $(OBJS)
	$(CC) $(OBJS) $(LIBS) -o $@ $(LDFLAGS)

clean:
	@rm -rf $(OBJS) $(PROGRAM)
