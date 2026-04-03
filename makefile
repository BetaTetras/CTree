all: bin/CTree

bin/CTree: src/CTree.o src/tools.o src/output.o
	gcc -Wall -Wextra -g -o bin/CTree src/CTree.o src/tools.o src/output.o -lpthread

src/CTree.o: src/CTree.c include/types.h include/tools.h include/output.h
	gcc -Wall -Wextra -g -Iinclude -c src/CTree.c -o src/CTree.o

src/tools.o: src/tools.c include/types.h include/tools.h
	gcc -Wall -Wextra -g -Iinclude -c src/tools.c -o src/tools.o

src/output.o: src/output.c include/types.h include/output.h include/tools.h
	gcc -Wall -Wextra -g -Iinclude -c src/output.c -o src/output.o

clean:
	rm -f src/CTree.o src/tools.o src/output.o bin/CTree