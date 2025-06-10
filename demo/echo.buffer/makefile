build: build/client build/server

build/client:
	gcc client.c -o client.out

build/server:
	gcc server.safe.c -o server.safe.out

run/client:
	clear
	./client.out

run/server: run/server/safe

run/server/safe:
	clear
	./server.safe.out
