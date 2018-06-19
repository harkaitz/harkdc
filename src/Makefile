all:
LIBS += -Wl,-Bstatic -luterm -luterm-socket -lserialport -Wl,-Bdynamic
include ../c-cpp.mk
include ../upload.mk
ifeq ($(SYSTEM_NAME),Windows)
SYSLIBS += -Wl,-Bstatic -lpthread -Wl,-Bdynamic -lws2_32 -lsetupapi
else
SYSLIBS += -lpthread
endif

## Devices.
DEVICE_SOURCES=$(shell echo devices/*.c)
$(call m_b_objects_lib,harkd.c): devices/devices.h
devices/devices.h: makeh $(DEVICE_SOURCES)
	sh makeh "const harkd_def_t" harkd_device_list $(DEVICE_SOURCES) > $@

TESTS_SOURCES=$(shell echo tests/*.c)
$(call m_b_objects_lib,harkd.c): tests/tests.h
tests/tests.h: makeh $(TESTS_SOURCES)
	echo '#include "../harkd-test.h"'                           > $@
	sh makeh 'const harkd_test_t' harkd_tests $(TESTS_SOURCES) >> $@

##
SOURCES=harkd.c $(DEVICE_SOURCES) $(TESTS_SOURCES)
HEADERS=harkd.h harkd-device.h



LIBRARY=$(call m_b_library,harkd)
PROGRAM=$(call m_b_executable,harkdc)
OBJECTS=$(call m_b_objects_lib,$(SOURCES))
ifeq ($(SYSTEM_NAME),Windows)
WINDRES  =$(if $(HOST),$(HOST)-windres,windres)
ICONOBJ  =$(call m_b_objects,icon.rc)
$(ICONOBJ): harkd.ico
	echo 'id ICON "$<"' > harkd.rc
	$(WINDRES) -i harkd.rc -o $@
	rm harkd.rc
else
ICONOBJ =
endif



$(OBJECTS):$(HEADERS)
$(OBJECTS):$(shell echo devices/*.h)

all: $(LIBRARY) $(PROGRAM) $(call m_b_incdir_pair,$(HEADERS))
$(LIBRARY): $(OBJECTS)
$(PROGRAM): $(call m_b_objects_lib,main.c) $(LIBRARY) $(ICONOBJ)
	$(call m_link_exe,$@,$^) $(SYSLIBS) $(ICONOBJ)
test: $(PROGRAM)
	$(PROGRAM)
test-server: $(PROGRAM)
	$(PROGRAM) -p 2000

all: $(OUTDIR)/bin/test-harkdc
$(OUTDIR)/bin/test-harkdc: test-harkdc
	cp $< $@



## Dist extras.
ifeq ($(SYSTEM_NAME),Windows)


$(OUTDIR)/share/harkdc/drivers/% : ../dist/%
	mkdir -p $(dir $@)
	cp $< $@
all: $(OUTDIR)/share/harkdc/drivers/CDM21228_Setup.exe
../dist/CDM21228_Setup.exe:
	mkdir -p ../dist
	wget -O $@.zip "http://www.ftdichip.com/Drivers/CDM/CDM21228_Setup.zip"
	7z x $@.zip -o../dist


endif


