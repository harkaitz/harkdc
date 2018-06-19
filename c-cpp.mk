#:: ## c-cpp.mk
#::
#:: Automatic rules for C/C++ developtment. This file requires `base.mk`
#::
#:: ### Object files. (Automatically generated)
#::
#:: ```
#:: $(call m_b_objects,<file>)     $TMPDIR/objs/<file>.o       AUTO-RULE
#:: $(call m_b_objects_lib,<file>) $tmpDIR/objs/<file>.l.o     AUTO-RULE
#:: ```
#::
#:: ### Link files.
#::
#:: ```
#:: $(call m_b_executable,<name>)  $OUTDIR/bin/<name><suffix>  AUTO-RULE
#:: $(call m_b_staticlib,<file>)   $OUTDIR/lib/lib<name><slib> AUTO-RULE
#:: $(call m_b_library,<file>)     $OUTDIR/lib/lib<name><slib> AUTO-RULE
#:: $(call m_b_dynamiclib,<file>)  $OUTDIR/lib/lib<name><dlib> AUTO-RULE
#:: ```
#::
#:: ### Includes.
#::
#:: ```
#:: $(call m_b_incdir_pair,<file>) $OUTDIR/include/<file>  AUTO-RULE
#:: ```
#::
#:: ### Functions.
#::
#:: ```
#:: m_link_lib=mkdir -p $(dir $@) && $(AR) rvs $(1) $(2)
#:: m_link_exe=mkdir -p $(dir $@) && $(CC) -o $(1) $(2) $(LDFLAGS) $(LIBS)
#:: ```
#::
# Find project root directory.
MKDIR=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
include $(MKDIR)/base.mk
help: help-c-cpp
DLIB         ?=.so
SLIB         ?=.a
SUFFIX       ?=
CFLAGS_LIBS  ?=
CXXFLAGS     ?= $(CFLAGS) $(CPPFLAGS)
CFLAGS       ?= $(CPPFLAGS)
help-c-cpp:
	@echo "================ c-cpp.mk ================="
	@echo "CC=$(CC)"
	@echo "CXX=$(CXX)"
	@echo "CFLAGS=$(CFLAGS)"
	@echo "CXXFLAGS=$(CXXFLAGS)"
	@echo "CFLAGS_LIBS=$(CFLAGS_LIBS) # CFLAGS for building libraries."
	@echo "LDFLAGS=$(LDFLAGS)"
	@echo "DLIB=$(DLIB)  (Objects: .l.o)"
	@echo "SLIB=$(SLIB)  (Objects: .l.o)"
	@echo "SUFFIX=$(SUFFIX) (Objects: .o)"
	@echo "OBJDIR=$(OBJDIR)"
	@echo "check-syntax : A little help for GNU/Emacs."




m_b_objects     =$(call m_tmpdir,$(1),objs/,.o)
m_b_objects_lib =$(call m_tmpdir,$(1),objs/,.l.o)
m_b_executable  =$(call m_outdir,$(1),bin/,$(SUFFIX))
m_b_staticlib   =$(call m_outdir,$(1),lib/lib,$(SLIB))
m_b_library     =$(call m_b_staticlib,$(1))
m_b_dynamiclib  =$(call m_outdir,$(1),lib/lib,$(DLIB))
m_b_incdir_pair =$(call m_outdir,$(1),include/$(PROJECT)/)


## BUILD C++
$(call m_b_objects_lib,%.cc) : %.cc
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< -I. $(CFLAGS_LIBS) $(CXXFLAGS)
$(call m_b_objects_lib,%.cpp) : %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< -I. $(CFLAGS_LIBS) $(CXXFLAGS)
$(call m_b_objects,%.cc) : %.cc
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< -I. $(CXXFLAGS)
$(call m_b_objects,%.cpp) : %.cpp
	@mkdir -p $(dir $@)
	$(CXX) -c -o $@ $< -I. $(CXXFLAGS)

## BUILD C
$(call m_b_objects,%.c) : %.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< -I. $(CFLAGS)
$(call m_b_objects_lib,%.c) : %.c
	@mkdir -p $(dir $@)
	$(CC) -c -o $@ $< -I. $(CFLAGS_LIBS) $(CFLAGS)

## STATIC LIBRARY.
m_link_lib=mkdir -p $(dir $@) && $(AR) rvs $(1) $(2)
m_link_exe=mkdir -p $(dir $@) && $(CC) -o $(1) $(2) $(LDFLAGS) $(LIBS) $(LIBS_$(SYSTEM_NAME))
$(call m_b_library,%):
	$(call m_link_lib,$@,$^)
$(call m_b_executable,%):
	$(call m_link_exe,$@,$^)
$(call m_b_incdir_pair,%.h): %.h
	mkdir -p `dirname $@` && cp $< $@
check-syntax:
	case "${CHK_SOURCES}" in                                                   \
	*.c|*.h)               $(CC)  -S -o /dev/null $(CFLAGS)   ${CHK_SOURCES};; \
	*.cc|*.cpp|*.hh|*.hpp) $(CXX) -S -o /dev/null $(CXXFLAGS) ${CHK_SOURCES};; \
	esac
