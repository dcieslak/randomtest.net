test:
	wget -qO - $$RANDOMTEST_URL?count=1 > /tmp/randomtest.counter
	sed "s!%RANDOMTEST_URL%!$(RANDOMTEST_URL)!g" nodejs-env.js randomtest-probe.js test1.js > /tmp/randomtest.js
	js /tmp/randomtest.js | sh -x
	wget -qO - $$RANDOMTEST_URL?count=1 >> /tmp/randomtest.counter
	awk 'FNR==1 {A=$$1} FNR==2 {B=$$1} END { if(B!=A+1) { print "New stacktrace not registered:" , A, B; exit(1)} }' /tmp/randomtest.counter
	
	sed "s!%RANDOMTEST_URL%!$(RANDOMTEST_URL)!g" test1.html > var/test1.html



