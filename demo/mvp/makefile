build:
	$(MAKE) build/empty
	$(MAKE) build/public
	$(MAKE) build/secret


build/empty:
	$(MAKE) build/empty/socket
	$(MAKE) build/empty/stdin

build/empty/socket:
	cd empty_socket && $(MAKE) build

build/empty/stdin:
	cd empty_stdin && $(MAKE) build

build/public:
	cd public && $(MAKE) build

build/secret:
	$(MAKE) build/secret/1
	$(MAKE) build/secret/2

build/secret/1:
	cd secret_1 && $(MAKE) build

build/secret/2:
	cd secret_2 && $(MAKE) build


run/empty/socket:
	cd empty_socket && $(MAKE) run/demo

run/empty/socket/client/public:
	cd empty_socket && $(MAKE) run/client/public

run/empty/socket/client/secret:
	cd empty_socket && $(MAKE) run/client/secret

run/empty/stdin:
	$(MAKE) run/empty/stdin/yes
	$(MAKE) run/empty/stdin/no

run/empty/stdin/yes:
	cd empty_stdin && $(MAKE) run/yes

run/empty/stdin/no:
	cd empty_stdin && $(MAKE) run/no

run/public:
	cd secret_1 && $(MAKE) run

run/secret/1:
	cd secret_1 && $(MAKE) run

run/secret/2:
	cd secret_2 && $(MAKE) run


clean:
	rm */*.out
