bulid: 
	mkdir -p ./bin
	gcc -o ./bin/jibim src/main.c
run: bulid
	./bin/jibim

