help:
	cat README.md


build:
	$(MAKE) build/mvp
	$(MAKE) build/echo

build/mvp:
	cd mvp && $(MAKE) build

build/echo:
	cd echo && $(MAKE) build


clean:
	$(MAKE) clean/mvp
	$(MAKE) clean/echo

clean/mvp:
	cd mvp && $(MAKE) clean

clean/echo:
	cd echo && $(MAKE) clean
