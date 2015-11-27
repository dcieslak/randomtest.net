WAR_DEPLOY_DIR=$(JBOSS_HOME)/server/default/deploy/randomtest1.war

all:
	mkdir -p classes
	javac  -source 1.6 -target 1.6 \
		-cp lib/servlet-api.jar -d classes \
		*.java

install:
	rm -rf $(WAR_DEPLOY_DIR)
	mkdir -p $(WAR_DEPLOY_DIR)/WEB-INF
	cp -r classes $(WAR_DEPLOY_DIR)/WEB-INF/
	sed 's!%RANDOMTEST_URL%!$(RANDOMTEST_URL)!' web.xml > $(WAR_DEPLOY_DIR)/WEB-INF/web.xml

test:
	wget -qO - http://localhost:8080/randomtest1/ExceptionTest || true

WEB-INF/classes/com/randomtest/RandomTestErrorHandler.class: RandomTestErrorHandler.java

clean:
	rm -rf WEB-INF/classes
	rm -rf $(JBOSS_HOME)/server/default/deploy/randomtest1.war


