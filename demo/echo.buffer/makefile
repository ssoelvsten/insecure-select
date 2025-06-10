build: build/client build/server

build/client:
	gcc client.c -o client.out

build/server:
	gcc server.safe.c -o server.safe.out
	gcc server.bad_close.c -o server.bad_close.out

run/client:
	clear
	./client.out

run/server: run/server/safe

run/server/safe:
	clear
	./server.safe.out

run/server/bad_close:
	clear
	./server.bad_close.out
