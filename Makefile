all:
	javac jwacom/WacomEvent.java
	javac jwacom/WacomListener.java
	javac jwacom/Wacom.java
	javah "jwacom.Wacom"
	jar cvf jwacom.jar jwacom/*.class
	gcc -c -I/usr/X11R6/include jwacom_Wacom.c
	gcc -L/usr/X11R6/lib -LX11 -lXi -shared -o libjwacom.so jwacom_Wacom.o

test:
	javac JWacomTest.java
	LD_LIBRARY_PATH=. java JWacomTest

clean:
	rm -rf jwacom.jar jwacom_Wacom.h jwacom_Wacom.o jwacom/*.class *.class libjwacom.so
