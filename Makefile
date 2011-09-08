INSTPATH=/lib64
INCPATH=/usr/include

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

install: lib
	@echo installing library to ${INSTPATH}
	@mkdir -p ${INSTPATH}
	@cp -f bin/libpts.so ${INSTPATH}
	@chmod 755 ${INSTPATH}/libpts.so
	@echo installing header to ${INCPATH}
	@cp -f src/pts.h ${INCPATH}

uninstall:
	@echo Deleting ${INSTPATH}/libpts.so
	@rm -f ${INSTPATH}/libpts.so
	@echo Deleting ${INCPATH}/pts.h
	@rm -f ${INCPATH}/pts.h

clean:
	rm -rf bin/ obj/ doc/
