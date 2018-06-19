help:
help: help-upload
upload:
ifneq ($(ZIPFILE),)
####
ssh-upload: $(ZIPFILE)
	@if test ! -n "$(SSH)";then echo "Set SSH variable. (ie <user>@<host>)"; false; fi
	cat $(ZIPFILE) | ssh $(SSH) \
	"cat > /tmp/t.zip && mkdir -p .local && cd .local && unzip -o /tmp/t.zip" 
endif
ssh-test-%: ssh-upload
	ssh $(SSH) $(shell echo '$@' | sed 's|^ssh-test-|.local$(PREFIX)/bin/|')
