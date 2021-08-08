ifeq ("$(shell uname)", "Linux") 
  OS="Unix"
endif

ifeq ("$(shell uname)", "FreeBSD") 
  OS="Unix"
endif

ifndef OS
  $(error Unsupported operating system)
endif

MAIN=glsl_vnoise
CC=g++
CFLAGS= -Wall -g
INCLUDES= -I../../inc -I../../../../inc -I/usr/X11R6/include
LFLAGS= 
SRCS = vnoise.cpp vnoise_init.cpp ../shared/data_path.cpp ../shared/read_text_file.cpp

ifeq ($(OS), "Unix")
  CFLAGS += -DUNIX
  LFLAGS += -L/usr/X11R6/lib -lXmu -lXi -lglut -lGLU -lGL
endif

OBJS = $(SRCS:.cpp=.o)

$(MAIN):  $(OBJS)
	$(CC) $(OBJS) -o $(MAIN) $(LFLAGS)
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)
	rm -f *~ $(MAIN)
