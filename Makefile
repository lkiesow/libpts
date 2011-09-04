lib: src/pts.c src/pts.h
	mkdir -p obj/ bin/
	gcc -Wall -c -fPIC src/pts.c -o obj/pts.o
	gcc -shared -Wl,-soname,libpts.so -o bin/libpts.so obj/pts.o

test: lib src/test.c
	mkdir -p bin/
	gcc src/test.c -lpts -Isrc -Lbin -o bin/test
	cp src/example-data/*.pts bin/

doc: Doxyfile src/pts.h
	mkdir -p doc
	doxygen Doxyfile

clean:
	rm -rf bin/ obj/ doc/
