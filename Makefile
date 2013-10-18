default: build 

build:
	make -C src/probes/c
	make -C src/probes/java

test: src/server/var
	make -C src/probes/c test
	make -C src/probes/php test
	make -C src/probes/java test
	wget -qO - $$RANDOMTEST_URL

src/server/var:
	mkdir src/server/var
	# PHP process (typically www-data) acess granted
	chmod o+w src/server/var

clean:
	make -C src/probes/c clean
	make -C src/probes/java clean
	rm -f src/server/var/*

sleep:
	sleep 3

all: clean build sleep test

-include local.mak

