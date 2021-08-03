MAIN=cg_ShinySprite

CC=g++

CFLAGS= -DUNIX -Wall -g 

INCLUDES= -I../../../../LIBS/inc -I../../inc -I../../../../inc -I/usr/X11R6/include

LFLAGS= -L/usr/X11R6/lib -lXmu -lXi -lCgGL -lCg -lglut -lGLU -lGL -lz -L../../../../LIBS/lib -lnvpng -lnvparse -lpthread

SRCS =  cg_CShinySprites.cpp cg_ShinySprites.cpp ../shared/pbuffer.cpp ../shared/nv_png.cpp ../shared/read_text_file.cpp ../shared/array_texture.cpp ../shared/data_path.cpp

OBJS = $(SRCS:.cpp=.o)

$(MAIN):  $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(MAIN) $(LFLAGS)
.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS)
	rm -f *~ $(MAIN)
