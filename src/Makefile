SRC = \
  vector.gresource.xml \
  vector.c

TARGET = vector

vector: vector.c vector.gresource.c
	gcc -g -o $@ `pkg-config --cflags gtk4` $^ `pkg-config --libs gtk4` -lm

%.gresource.c:%.gresource.xml
	glib-compile-resources $< --target=$@ --generate-source

clean:
	-rm -rf ${TARGET} ${TARGET}.gresource.c
