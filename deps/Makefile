uname_S := $(shell sh -c 'uname -s 2>/dev/null || echo not')

PREFIX = `pwd`/../target

ifeq ($(uname_S),SunOS)
	LIBTYPE=shared
	LIBSUFFIX=so
else # Linux, Darwin
	LIBTYPE=static
	LIBSUFFIX=a
endif

SGVER=0.90
EVVER=4.15

LIBEV=target/lib/libev.$(LIBSUFFIX)
LIBSTATGRAB=target/lib/libstatgrab.$(LIBSUFFIX)

all: $(LIBSTATGRAB) $(LIBEV)

MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

$(LIBSTATGRAB):
	@printf '[%b]\n' $(MAKECOLOR)libstatgrab$(ENDCOLOR)
	@wget -q -O libstatgrab-$(SGVER).tar.gz http://ftp.i-scream.org/pub/i-scream/libstatgrab/libstatgrab-$(SGVER).tar.gz
	@tar -xpf libstatgrab-$(SGVER).tar.gz
	@cd libstatgrab-$(SGVER) && ./configure --prefix=$(PREFIX) --enable-$(LIBTYPE)
	@cd libstatgrab-$(SGVER) && make && make install
	@echo

$(LIBEV):
	@printf '[%b]\n' $(MAKECOLOR)libev$(ENDCOLOR)
	@wget -q -O libev-$(EVVER).tar.gz http://dist.schmorp.de/libev/libev-$(EVVER).tar.gz
	@tar -xpf libev-$(EVVER).tar.gz
	@cd libev-$(EVVER) && ./configure --prefix=$(PREFIX) --enable-$(LIBTYPE)
	@cd libev-$(EVVER) && make && make install
	@echo

clean:
	@rm -rf target libstatgrab* libev*
