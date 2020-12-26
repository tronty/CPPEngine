TARGETforest=forest.bin
TARGETtreeview=treeview.bin

SDL_CFLAGS  := $(shell sdl-config --cflags)
SDL_LDFLAGS := $(shell sdl-config --libs)

COBJFLAGS = -fmax-errors=100 -std=gnu99 -D_DEBUG -DDEBUG -DGLSL1_1 -DUSE_HLSL_SHADERS -DMESHRENDERER2_SIMPLER -I../../../Dependencies/BoostWorkaround -I../../../Libraries/assimp-3.2/include -I../../../Libraries/Vulkan/external -I../../../Libraries/Vulkan/include -I../../../Libraries/STX/include -I../../../Libraries/Image3 -I../../../Libraries/Framework3/include -I../../../Libraries/NSGUI -I../../../Libraries/MeshRenderer2 -I../../../Libraries/tinyxml/include -I../../../Libraries/lua-5.3.4/src -I../../../Libraries/FW3/include -I../../../Libraries/IndieLib/IndieLib/Common -I../../../Libraries/hlsl2glslfork/include -I../../../Libraries/otl/include -I../../../Samples/SimpleTexture -I../../../Libraries/STX/include -I../../../Libraries/Framework3/include -w
CFLAGS += $(PIC3) -fpermissive $(COBJFLAGS)
	
CXXFLAGS += $(CFLAGS)
CPPFLAGS += $(CFLAGS)

SRCStreeview_ =	./examples/treeview/treeview.cpp 
SRCSforest_   =	./examples/forest/treefactory.cpp \
		./examples/forest/main.cpp \
		./examples/forest/forestgamelet.cpp 
SRCS	     =	./src/renderer/shader_cg.cpp \
		./src/renderer/sdlframework.cpp \
		./src/renderer/material.cpp \
		./src/renderer/vertexlist.cpp \
		./src/renderer/renderer.cpp \
		./src/weberarbaro/arbarostem.cpp \
		./src/weberarbaro/arbaroleaf.cpp \
		./src/weberarbaro/arbarotree.cpp \
		./src/weberarbaro/arbaroweber.cpp \
		./src/mesher/leafmesher.cpp \
		./src/mesher/stemmesher.cpp \
		./src/mesher/treemesher.cpp \
		./src/mesher/cylmesher.cpp \
		./src/mesher/fixedevenstemmesher.cpp \
		./src/tinyxml/tinyxml.cpp \
		./src/tinyxml/tinystr.cpp \
		./src/tinyxml/tinyxmlerror.cpp \
		./src/tinyxml/tinyxmlparser.cpp \
		./src/utils/otpolylinespline3.cpp \
		./src/utils/otpolylinespline.cpp \
		./src/weber/clonefactory.cpp \
		./src/weber/weber.cpp \
		./src/weber/stemfactory.cpp \
		./src/weber/tree.cpp

SRCStreeview = $(SRCS) $(SRCStreeview_)
SRCSforest   = $(SRCS) $(SRCSforest_)
	
%.o: %.cpp
	g++ -g3 -O0 -I/usr/include/SDL -Iinclude -Iexamples/treeview -Iexamples/forest -fexceptions -fnon-call-exceptions $(CPLUSPLUS) $(CPPFLAGS) -o $@ -c $<
	
OBJStreeview=$(patsubst %.cpp,%.o,$(patsubst %.cxx,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(filter %.cpp %.cxx %.cc %c ,$(SRCStreeview))))))
FILEStreeview=$(patsubst %.cxx,%.cxx~,$(patsubst %.cpp,%.cpp~,$(patsubst %.cc,%.cc~,$(patsubst %.c,%.c~,$(filter %.c %.cc %.cpp %.cxx ,$(SRCStreeview))))))
	
OBJSforest=$(patsubst %.cpp,%.o,$(patsubst %.cxx,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(filter %.cpp %.cxx %.cc %c ,$(SRCSforest))))))
FILESforest=$(patsubst %.cxx,%.cxx~,$(patsubst %.cpp,%.cpp~,$(patsubst %.cc,%.cc~,$(patsubst %.c,%.c~,$(filter %.c %.cc %.cpp %.cxx ,$(SRCSforest))))))

$(TARGETtreeview): $(OBJStreeview)
	gcc $(LDFLAGS) $(SDL_LDFLAGS) -g3 -O0 $(OBJStreeview) -L/usr/local/lib -lGL -lGLU -lSDL -lSDL_image $(CGLIBS) $(PTHREAD) $(X11) -lstdc++ -lz -lm -ldl -o $@

$(TARGETforest): $(OBJSforest)
	gcc $(LDFLAGS) $(SDL_LDFLAGS) -g3 -O0 $(OBJSforest) -L/usr/local/lib -lGL -lGLU -lSDL -lSDL_image $(CGLIBS) $(PTHREAD) $(X11) -lstdc++ -lz -lm -ldl -o $@

forest: $(TARGETforest)
treeview: $(TARGETtreeview)

clean:
	rm -f *.bin
	find . -name "*.o" -type f -delete

