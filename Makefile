all:
	make -C src/probes/c

clean:
	make -C src/probes/c clean

-include local.mak

