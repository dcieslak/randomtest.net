PROBE_NAME=randomtest-probe

build: $(PROBE_NAME).so test1.exe

$(PROBE_NAME).so: $(PROBE_NAME).c
	$(CXX) `curl-config --cflags` -c -Wall -Werror -fpic --pedantic-errors $(PROBE_NAME).c
	$(CXX) -shared -o $(PROBE_NAME).so $(PROBE_NAME).o `curl-config --libs`

test1.exe: test1.cpp
	$(CXX) -rdynamic -Wall -Werror --pedantic-errors test1.cpp -o test1.exe

test:
	LD_PRELOAD=./$(PROBE_NAME).so ./test1.exe || true

clean:
	rm -f *.o *.so *.exe

