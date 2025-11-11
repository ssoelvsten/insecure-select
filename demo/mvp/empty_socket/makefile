build: build/demo build/client

build/demo:
	gcc demo.c -o demo.out

build/client:
		gcc client.c -o client.out

run/demo:
	clear
	rm -f /tmp/socket_demo.*
	./demo.out

run/client/public:
	clear
	./client.out public

run/client/secret:
	clear
	./client.out secret

clean:
	rm *.out
