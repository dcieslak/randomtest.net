all: build src/server/var test

build:
	make -C src/probes/c

test:
	make -C src/probes/c test
	make -C src/probes/php test
	wget -qO - $$RANDOMTEST_URL

src/server/var:
	mkdir src/server/var
	# PHP process (typically www-data) acess granted
	chmod o+w src/server/var

clean:
	make -C src/probes/c clean

-include local.mak

