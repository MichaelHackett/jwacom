JDK_HOME = /usr/lib/jvm/java-6-openjdk
BUILD_DIR = build
CLASS_DIR = $(BUILD_DIR)/classes
TEST_CLASS_DIR = $(BUILD_DIR)/classes-test

all: library runtest

library: jar $(BUILD_DIR)/libjwacom.so

jar: $(BUILD_DIR) $(BUILD_DIR)/jwacom.jar

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CLASS_DIR):
	mkdir -p $(CLASS_DIR)

$(TEST_CLASS_DIR):
	mkdir -p $(TEST_CLASS_DIR)

$(BUILD_DIR)/jwacom.jar: $(CLASS_DIR)/jwacom/WacomEvent.class $(CLASS_DIR)/jwacom/WacomListener.class $(CLASS_DIR)/jwacom/Wacom.class
	jar cvf $(BUILD_DIR)/jwacom.jar $(CLASS_DIR)/jwacom/*.class

$(CLASS_DIR)/jwacom/WacomEvent.class: $(CLASS_DIR) jwacom/WacomEvent.java
	javac -d $(CLASS_DIR) jwacom/WacomEvent.java

$(CLASS_DIR)/jwacom/WacomListener.class: $(CLASS_DIR) jwacom/WacomListener.java
	javac -d $(CLASS_DIR) jwacom/WacomListener.java

$(CLASS_DIR)/jwacom/Wacom.class: $(CLASS_DIR) jwacom/Wacom.java
	javac -d $(CLASS_DIR) jwacom/Wacom.java

$(BUILD_DIR)/jwacom_Wacom.h: $(CLASS_DIR)/jwacom/Wacom.class
	javah -d $(BUILD_DIR) -classpath $(CLASS_DIR) "jwacom.Wacom"

$(BUILD_DIR)/libjwacom.so: $(BUILD_DIR)/jwacom_Wacom.o
	gcc -LX11 -lXi -shared -o $(BUILD_DIR)/libjwacom.so $(BUILD_DIR)/jwacom_Wacom.o

$(BUILD_DIR)/jwacom_Wacom.o: jwacom_Wacom.c $(BUILD_DIR)/jwacom_Wacom.h
	gcc -c -fPIC -I$(JDK_HOME)/include -I$(BUILD_DIR) -o $(BUILD_DIR)/jwacom_Wacom.o jwacom_Wacom.c

runtest: test $(BUILD_DIR)/libjwacom.so
	LD_LIBRARY_PATH=$(BUILD_DIR) java -classpath $(TEST_CLASS_DIR):$(BUILD_DIR)/jwacom.jar JWacomTest

test: $(TEST_CLASS_DIR)/JWacomTest.class

$(TEST_CLASS_DIR)/JWacomTest.class: $(TEST_CLASS_DIR) JWacomTest.java $(BUILD_DIR)/jwacom.jar
	javac -d $(TEST_CLASS_DIR) JWacomTest.java

clean:
	rm -rf $(BUILD_DIR)

