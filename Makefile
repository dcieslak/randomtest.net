all: build test

build:
	make -C src/probes/c

test:
	make -C src/probes/c test
	make -C src/probes/php test

clean:
	make -C src/probes/c clean

-include local.mak

