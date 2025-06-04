build: build/demo build/connect

build/demo:
	gcc demo.c -o demo.out

build/connect:
		gcc connect.c -o connect.out

run/demo:
	clear
	rm -f /tmp/socket_demo.*
	./demo.out

run/connect/public:
	clear
	./connect.out public

run/connect/secret:
	clear
	./connect.out secret
