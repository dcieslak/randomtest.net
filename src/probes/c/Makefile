PROBE_NAME=randomtest-probe
COMMON_OBJS=randomtest-common.o

build: $(PROBE_NAME).so randomtest-stdin test1.exe

$(PROBE_NAME).so: $(PROBE_NAME).c $(COMMON_OBJS)
	$(CXX) `pkg-config --cflags libcurl` -c -Wall -Werror -fpic --pedantic-errors $<
	$(CXX) -shared $(PROBE_NAME).o $(COMMON_OBJS) `pkg-config --libs libcurl` -o $(PROBE_NAME).so

%.o: %.c
	$(CXX) -rdynamic -Wall -Werror --pedantic-errors $< -c -o $@

randomtest-stdin: randomtest-stdin.c $(COMMON_OBJS)
	$(CXX) -rdynamic -Wall -Werror --pedantic-errors $< $(COMMON_OBJS) `pkg-config --libs libcurl` -o $@

test1.exe: test1.cpp
	$(CXX) -rdynamic -Wall -Werror --pedantic-errors $< -o $@

test: test1.exe $(PROBE_NAME).so randomtest-stdin
	echo RANDOMTEST_URL=$(RANDOMTEST_URL)
	LD_PRELOAD=./$(PROBE_NAME).so ./test1.exe || true
	printf "One line\nSecond line\n\n" | ./randomtest-stdin
	wget -qO probe.log "$(RANDOMTEST_URL)"
	test -f "$(RANDOMTEST_FILE)"
	grep -q test1.exe probe.log
	grep -q 'Second line' probe.log
	grep -q test1.exe "$(RANDOMTEST_FILE)"
	grep -q 'Second line' "$(RANDOMTEST_FILE)"
	grep -q MyClass2::myMethod2 "$(RANDOMTEST_FILE)"

clean:
	rm -f *.o *.so *.exe *.log

