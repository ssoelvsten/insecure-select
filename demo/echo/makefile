build: build/client build/server


build/client:
	gcc client.c -o client.out


build/server:
	$(MAKE) build/server/buffer.close
	$(MAKE) build/server/buffer.safe
	$(MAKE) build/server/buffer.share
	$(MAKE) build/server/cf.drop_1
	$(MAKE) build/server/cf.drop_2
	$(MAKE) build/server/cf.first-served
	$(MAKE) build/server/cf.throttle
	$(MAKE) build/server/safe

build/server/buffer.close:
	cd buffer.close && $(MAKE) build

build/server/buffer.safe:
	cd buffer.safe && $(MAKE) build

build/server/buffer.share:
	cd buffer.share && $(MAKE) build

build/server/cf.drop_1:
	cd cf.drop_1 && $(MAKE) build

build/server/cf.drop_2:
	cd cf.drop_2 && $(MAKE) build

build/server/cf.first-served:
	cd cf.first-served && $(MAKE) build

build/server/cf.throttle:
	cd cf.throttle && $(MAKE) build

build/server/safe:
	cd safe && $(MAKE) build


run/client:
	clear
	./client.out


run/server/buffer.bad_close:
	cd buffer.bad_close && $(MAKE) run

run/server/buffer.safe:
	cd buffer.safe && $(MAKE) run

run/server/buffer.share:
	cd buffer.share && $(MAKE) run

run/server/cf.drop_1:
	cd cf.drop_1 && $(MAKE) run

run/server/cf.drop_2:
	cd cf.drop_2 && $(MAKE) run

run/server/cf.first-served:
	cd cf.first-served && $(MAKE) run

run/server/cf.throttle:
	cd cf.throttle && $(MAKE) run

run/server/safe:
	cd safe && $(MAKE) run

clean:
	rm *.out
	rm */*.out
