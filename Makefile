INSTPATH=/lib
INCPATH=/usr/include

lib: src/pts.c src/pts.h
	mkdir -p obj/ bin/
	gcc -Wall -c -fPIC \
		"-DVERSION=\"`git log -1 | sed -n 's/commit \(.*\)/\1/p'`\"" \
		"-DMODDATE=\"`git log -1 | sed -n 's/Date: *\(.*\)/\1/p'`\"" src/pts.c -o obj/pts.o
	gcc -shared -Wl,-soname,libpts.so -o bin/libpts.so obj/pts.o
	sed "s/→→VERSION←←/`git log -1 | sed -n 's/commit \(.*\)/\1/p'`/" < src/pts.h \
		| sed "s/→→MODIFIED←←/`git log -1 | sed -n 's/Date: *\(.*\)/\1/p'`/" > bin/pts.h

test: lib src/test.c
	mkdir -p bin/
	gcc src/test.c -lpts -Isrc -Lbin -o bin/test
	cp src/example-data/*.pts bin/

doc: Doxyfile lib
	cd bin ;\
	mkdir -p doc ;\
	doxygen ../Doxyfile

install: lib
	@echo Installing library to ${INSTPATH}…
	@mkdir -p ${INSTPATH}
	@cp -f bin/libpts.so ${INSTPATH}
	@chmod 755 ${INSTPATH}/libpts.so
	@echo Installing header to ${INCPATH}…
	@cp -f bin/pts.h ${INCPATH}

uninstall:
	@echo Deleting »${INSTPATH}/libpts.so«…
	@rm -f ${INSTPATH}/libpts.so
	@echo Deleting »${INCPATH}/pts.h«…
	@rm -f ${INCPATH}/pts.h

clean:
	rm -rf bin/ obj/
