# Default rule.
all:
ifeq ($(shell test ! -f src/Makefile || echo yes),yes)
%:
	$(MAKE) -C src $@
endif
install-targets:
	cd ../uterm && make install-targets
	hrkc -t i686-w64-mingw32 -g make
	make distclean install PREFIX=
	hrkc -g make
	make distclean install PREFIX=
