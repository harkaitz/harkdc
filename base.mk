#:: ## base.mk
#::
#:: This file contains some common makefile rules for any project.
#::
#:: ```
#:: $(call m_outdir,<file>)      -> $OUTDIR/<file>
#:: $(call m_tmpdir,<file>)      -> $TMPDIR/<file>
#:: $(call m_etcdir,<file>)      -> $ETCDIR/<file>
#:: ```
#::

help:
all:
clean:
# Find project root directory.
MKDIR=$(shell dirname $(realpath $(lastword $(MAKEFILE_LIST))))
ifeq ($(shell basename $(MKDIR)),mk)
PRJDIR=$(shell dirname $(MKDIR))
else
PRJDIR=$(MKDIR)
endif
# Read configuration.
CONFIG=$(MKDIR)/config.mk
ifeq ($(shell test ! -f $(CONFIG) || echo yes),yes)
include $(CONFIG)
endif
# Set project names.
PROJECT ?=$(notdir $(PRJDIR))
VERSION ?=0.1-alpha
HOST    ?=$(shell uname -s)
BUILD   ?=$(shell uname -s)
BDIR    ?=/tmp/build/$(PROJECT)-$(VERSION)-$(HOST)
DDIR    ?=/tmp/build
PREFIX  ?=/usr/local
ifeq ($(HOST),$(BUILD))
DESTDIR ?=$(HOME)/.local
else
DESTDIR ?=$(HOME)/.local/$(HOST)
endif
OUTDIR  ?=$(BDIR)$(PREFIX)
TMPDIR   =$(BDIR)/tmp
ETCDIR   =$(BDIR)/etc

ZIPFILE ?=$(DDIR)/$(PROJECT)-$(VERSION)-$(HOST).zip

help: help-base
help-base:
	@echo "====== base.mk ==================================="
	@echo "PRJDIR=$(PRJDIR)"
	@echo "CONFIG=$(CONFIG)"
	@echo ""
	@echo "PROJECT=$(PROJECT)"
	@echo "VERSION=$(VERSION)"
	@echo ""
	@echo "HOST   =$(HOST)"
	@echo "BDIR   =$(BDIR)"
	@echo "PREFIX =$(PREFIX)"
	@echo "DESTDIR=$(DESTDIR)"
	@echo "OUTDIR =$(OUTDIR)"
	@echo "TMPDIR =$(TMPDIR) [Do not set]"
	@echo "ETCDIR =$(ETCDIR)"
	@echo ""
	@echo "DDIR   =$(DDIR)"
	@echo "ZIPFILE=$(ZIPFILE)"
	@echo "zip       : Create zip file (PREFIX inside)."
	@echo "install   : Create zip file and extract in DESTDIR."
	@echo "list      : List files in BDIR."
	@echo "distclean : Remove all in BDIR."
	@echo "clean     : Remove all in BDIR/tmp"
	@echo "help-dev  : Print help about how to use this .mk files."
	@echo "template  : Update .mk templates."
	@echo ""
help-dev:
	@sed -n 's|^#::||p' $(MKDIR)/*.mk
template:
	@cd $(MKDIR) && for n in *.mk;do hrknew -f make/$$n || true;done

all: disable-all-root
disable-all-root:
	@if test "$(USER)" = "root";then           \
	    echo 'Run `make all` as common user.'; \
	    false;                                 \
	fi
zip: $(ZIPFILE)
$(ZIPFILE): all
	@mkdir -p $(DDIR) $(BDIR)
	@echo "Creating $(ZIPFILE) ..."
	@rm -f $(ZIPFILE)
	@cd $(BDIR) && zip -r $(ZIPFILE) `ls | sed '/^tmp$$/d'`
install: $(ZIPFILE)
	@mkdir -p $(DESTDIR)
	unzip -o $< -d $(DESTDIR)
list:
	mkdir -p $(BDIR)
	@cd $(BDIR) && find -type f -iregex '.*'
distclean:
	rm -fr $(BDIR)
clean:
	rm -fr $(BDIR)/tmp
m_outdir=$(addsuffix $(3),$(addprefix $(OUTDIR)/$(2),$(1)))
m_tmpdir=$(addsuffix $(3),$(addprefix $(TMPDIR)/$(2),$(1)))
m_etcdir=$(addsuffix $(3),$(addprefix $(ETCDIR)/$(2),$(1)))
$(OUTDIR)/%: %
	@mkdir -p $(dir $@)
	cp -r $< $@
