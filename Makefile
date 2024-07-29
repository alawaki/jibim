build: src/main.c
	mkdir -p ./bin
	gcc -o ./bin/jibim src/main.c
run: build
	./bin/jibim
test: src/test.c
	mkdir -p ./bin
	gcc -o ./bin/test src/test.c
	./bin/test
clean:
	rm -rf bin
