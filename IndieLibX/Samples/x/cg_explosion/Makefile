ifeq ("$(shell uname)", "Darwin")
  OS="MacOSX"
endif

ifeq ("$(shell uname)", "Linux") 
  OS="Unix"
endif

ifeq ("$(shell uname)", "FreeBSD") 
  OS="Unix"
endif

ifndef OS
  $(error Unsupported operating system)
endif

MAIN=cg_explosion
CC=g++
CFLAGS= -Wall -g
INCLUDES= -I../../inc -I../../../../LIBS/inc -I../../../../inc -I/usr/X11R6/include
LFLAGS= -lz -L../../../../LIBS/lib -lnvpng
SRCS = cg_Cdisc.cpp cg_Cexplosion.cpp cg_explosion.cpp

ifeq ($(OS), "Unix")
  CFLAGS += -DUNIX
  LFLAGS += -L/usr/X11R6/lib -lXmu -lXi -lCgGL -lCg -lglut -lGLU -lGL
endif

ifeq ($(OS), "MacOSX")
  CFLAGS += -DMACOS
  LFLAGS += -L/usr/lib -lobjc -framework OpenGL -framework GLUT -framework Cg
endif

OBJS = $(SRCS:.cpp=.o)

$(MAIN):  $(OBJS)
	$(CC) $(OBJS) -o $(MAIN) $(LFLAGS)
ifeq ($(OS), "MacOSX")
	perl ../build_app.pl $(MAIN)
endif
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)
	rm -f *~ $(MAIN)
ifeq ($(OS), "MacOSX")
	rm -rf $(MAIN).app
endif
