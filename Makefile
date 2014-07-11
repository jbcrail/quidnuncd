all:
	@cd deps && $(MAKE) $@
	@cd src && $(MAKE) $@

clean:
	@cd deps && $(MAKE) $@
	@cd src && $(MAKE) $@
