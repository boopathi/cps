all:
	mkdir -p build
	cc -o build/cps cps.c

install:
	cp build/cps /usr/bin/

clean:
	rm -rf build
