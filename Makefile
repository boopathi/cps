all:
	cc -o cps cps.c

install:
	cp cps /usr/bin/

clean:
	rm -f cps
