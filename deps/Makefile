uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

PREFIX = `pwd`/../target

ifeq ($(uname_S),SunOS)
	LIBSUFFIX=so
	CFGFLAGS=--enable-shared --disable-static
	AM_CFLAGS=-DECB_NO_SMP # Disable memory barriers for Solaris
else # Linux, Darwin, FreeBSD
	LIBSUFFIX=a
	CFGFLAGS=--enable-static --disable-shared
endif

SGVER=0.91
EVVER=4.19

LIBEV=target/lib/libev.$(LIBSUFFIX)
LIBSTATGRAB=target/lib/libstatgrab.$(LIBSUFFIX)

all: $(LIBSTATGRAB) $(LIBEV)

MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

$(LIBSTATGRAB):
	@printf '[%b]\n' $(MAKECOLOR)libstatgrab$(ENDCOLOR)
	@wget -q -O libstatgrab-$(SGVER).tar.gz http://ftp.i-scream.org/pub/i-scream/libstatgrab/libstatgrab-$(SGVER).tar.gz
	@tar -xpf libstatgrab-$(SGVER).tar.gz
	@cd libstatgrab-$(SGVER) && ./configure --prefix=$(PREFIX) --disable-tests $(CFGFLAGS)
	@cd libstatgrab-$(SGVER) && make && make install
	@echo

$(LIBEV):
	@printf '[%b]\n' $(MAKECOLOR)libev$(ENDCOLOR)
	@wget -q -O libev-$(EVVER).tar.gz http://dist.schmorp.de/libev/Attic/libev-$(EVVER).tar.gz
	@tar -xpf libev-$(EVVER).tar.gz
	@cd libev-$(EVVER) && ./configure --prefix=$(PREFIX) $(CFGFLAGS)
	@cd libev-$(EVVER) && make AM_CFLAGS=$(AM_CFLAGS) && make install
	@echo

clean:
	@rm -rf target libstatgrab* libev*
