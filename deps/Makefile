PREFIX = `pwd`/../target

all: target/lib/libstatgrab.a target/lib/libev.a

MAKECOLOR="\033[32;1m"
ENDCOLOR="\033[0m"

target/lib/libstatgrab.a:
	@printf '[%b]\n' $(MAKECOLOR)libstatgrab$(ENDCOLOR)
	@wget -q -O libstatgrab-0.90.tar.gz http://ftp.i-scream.org/pub/i-scream/libstatgrab/libstatgrab-0.90.tar.gz
	@tar -xpf libstatgrab-0.90.tar.gz
	@cd libstatgrab-0.90 && ./configure --prefix=$(PREFIX) --enable-static
	@cd libstatgrab-0.90 && make && make install
	@echo

target/lib/libev.a:
	@printf '[%b]\n' $(MAKECOLOR)libev$(ENDCOLOR)
	@wget -q -O libev-4.15.tar.gz http://dist.schmorp.de/libev/libev-4.15.tar.gz
	@tar -xpf libev-4.15.tar.gz
	@cd libev-4.15 && ./configure --prefix=$(PREFIX) --enable-static
	@cd libev-4.15 && make && make install
	@echo

clean:
	@rm -rf target libstatgrab* libev*
