bin/testmap: Include/hashmap.h Include/hashmap.inc src/test_hashmap.cpp | bin
	g++ -DDEBUG -g -O0 -o bin/testmap -I Include src/test_hashmap.cpp

test: bin/testmap
	./bin/testmap

bin: 
	mkdir bin

clean:
	@rm -rf bin
