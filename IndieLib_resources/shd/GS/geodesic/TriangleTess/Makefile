CPPFLAGS=-g -pthread -Ilib/vectormath -Ilib/glew -Ilib/glsw -Ilib/libpez

LDFLAGS=-g
LDLIBS=

geodesic: glew.o bstrlib.o glsw.o Geodesic.o pez.o 
	g++ $(LDFLAGS) -o geodesic glew.o bstrlib.o glsw.o pez.o Geodesic.o $(LDLIBS)

pez.o:
	g++ $(CPPFLAGS) -c lib/libpez/pez.linux.c

glsw.o:
	g++ $(CPPFLAGS) -c lib/glsw/glsw.c

bstrlib.o:
	g++ $(CPPFLAGS) -c lib/glsw/bstrlib.c

glew.o:
	g++ $(CPPFLAGS) -c lib/glew/glew.c

all:geodesic

clean:
	rm -fr geodesic *.o

