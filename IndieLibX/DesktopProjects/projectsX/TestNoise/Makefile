NAME = TestNoise
FILEEXT = ".bin"
OS = Ubuntu

ifeq ($(FILEEXT), ".so")
ifeq ("Darwin","$(OS)")
TARGET = ../../../bin/$(NAME).app/Contents/MacOS/lib$(NAME).so
else
TARGET = ../../../bin/lib$(NAME).so
endif
else ifeq ($(FILEEXT), ".a")
TARGET = ../../../lib/lib$(NAME).a
else ifeq ($(FILEEXT), ".app")
TARGETDIR = "../../../bin/$(NAME).app/Contents/MacOS"
TARGET = $(TARGETDIR)/$(NAME)
else ifeq ($(FILEEXT), ".bin")
TARGETDIR = "../../../bin"
TARGET = $(TARGETDIR)/$(NAME).bin
endif

MNAME := $(shell uname -m | tr "A-Z" "a-z")
ifeq ($(MNAME),i686)
BITNESS = "32"
LIBDIR=../../../../IndieLibX_Libraries/IndieLibX_AllLibs_LNX32
else
BITNESS = "64"
ifeq ("Darwin","$(OS)")
LIBDIR=../../../../IndieLibX_Libraries/IndieLibX_AllLibs_OSX
else
LIBDIR=../../../../IndieLibX_Libraries/IndieLibX_AllLibs_LNX64
endif
endif

LDFLAGS += -L$(LIBDIR)

ifeq ("Debian","$(OS)")
	export LibDir="Linux$(BITNESS)"
ifeq ("True","$(GLES2)")
	export OPENGL = -lGLESv2 -lGL -lGLU -lglut 
else ifeq ("True","$(GLES3_2)")
	export OPENGL = -lGLESv3 -lGL -lGLU -lglut 
else
	export OPENGL = -lGL -lGLU -lglut 
endif
	export SDL = -lSDL2 -lSDL2_image -lopenal
	export X11 = -lX11
	CFLAGS += -DOS_Debian
else ifeq ("Darwin","$(OS)")
	CFLAGS += -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$(MACOSX_VERSION).sdk
	LDFLAGS += -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX$(MACOSX_VERSION).sdk

	CFLAGS += -I/usr/local/include -I/usr/X11R6/include
	LDFLAGS += -L/usr/local/lib -L/usr/X11R6/lib
	export CPLUSPLUS = -D__cplusplus
	export LibDir="Mac64"
	export SDL = -lSDL2 -lSDL2_image -lX11
	export FRAMEWORKS = -framework OpenGL -framework OpenAL
else
	
	export CGLIBS = -lCg -lCgGL
	export CFLAGS += -DHAS_GLCG
			
ifeq ("Darwin","$(OS)")
	export OPENGL =
else ifeq ("True","$(GLES2)")
	export OPENGL = -lGLESv2 -lGL -lGLU -lglut 

else ifeq ("True","$(GLES3_2)")
	export OPENGL = -lGLESv3 -lGL -lGLU -lglut 
else
	export OPENGL = -lGL -lGLU -lglut 
endif
	export SDL = -lSDL2 -lSDL2_image -lopenal
	export X11 = -lX11
	#-L/usr/lib/i386-linux-gnu -lusb -ludev
	export LibDir="Linux$(BITNESS)"
endif

CXXFLAGS += -std=gnu++0x
CPPFLAGS += -std=gnu++0x

ifneq ("Darwin","$(OS)")
CFLAGS += -DLINUX -D_LINUX
#CFLAGS += -D__linux__
CFLAGS += -Wno-enum-compare
CFLAGS += -rdynamic 
endif





ifeq ($(OS), "Darwin")
ifeq ($(MNAME),i686)
ARCH = -arch i386 -mmacosx-version-min=10.8
else
ARCH = -arch x86_64 -mmacosx-version-min=10.8
endif
ARCH = -arch x86_64 -mmacosx-version-min=10.8
endif

ifeq ("Darwin","$(OS)")
NARROWING = -stdlib=libc++ -std=c++11 -Wno-c++11-narrowing -Wno-address-of-packed-member -Wno-address-of-temporary
else
NARROWING =
endif

ifeq ($(OS), "Darwin")
export DEVELOPER_DIR=/Applications/Xcode.app/Contents/Developer
# xcrun clang -arch i386 -arch x86_64 -stdlib=libc++ $(NARROWING) -mmacosx-version-min=10.8
export CC  = xcrun --sdk macosx$(MACOSX_VERSION) clang $(ARCH)
export GCC = xcrun --sdk macosx$(MACOSX_VERSION) clang $(ARCH)
export GPP = xcrun --sdk macosx$(MACOSX_VERSION) clang $(ARCH)
export AR  = xcrun --sdk macosx$(MACOSX_VERSION) ar
export LD  = xcrun --sdk macosx$(MACOSX_VERSION) ld
export RANLIB = xcrun --sdk macosx$(MACOSX_VERSION) ranlib
else ifeq ($(OS), "Windows")
export CC  = mingw32-gcc.exe
export GCC = mingw32-gcc.exe
export GPP = mingw32-g++.exe
export AR  = mingw32-gcc-ar.exe
export LD  = ld.exe
export RANLIB = mingw32-gcc-ranlib.exe
else
export CC  = /usr/bin/cc
export GCC = /usr/bin/gcc
export GPP = /usr/bin/g++
export AR  = /usr/bin/ar
export LD  = /usr/bin/ld
export RANLIB = /usr/bin/ranlib
endif

DEL=/bin/rm -fr

ifeq ($(OS), "Darwin")
#ifneq ($(MNAME),i686)
CFLAGS += -Dx86_64
#endif

FRAMEWORKS+=-F ../../../Dependencies/SDL2-2.0.14 -framework SDL2
LDFLAGS += -L ../../../Dependencies/Molten-0.18.0/MoltenVK/macOS
endif

CFLAGS += --trigraphs -Wno-comment -Wno-format -Wno-write-strings

ifeq ($(FILEEXT), ".so")
PIC3 = -fPIC
endif

COBJFLAGS = -fmax-errors=100 -std=gnu99 -D_DEBUG -DDEBUG -DGLSL1_1 -DUSE_HLSL_SHADERS -DMESHRENDERER2_SIMPLER -I../../../Dependencies/BoostWorkaround -I../../../Libraries/assimp-5.0.1/include -I../../../Libraries/assimp-5.0.1/contrib/irrXML -I../../../Libraries/Vulkan/external -I../../../Libraries/Vulkan/include -I../../../Libraries/STX/include -I../../../Libraries/Image3 -I../../../Libraries/Framework/include -I../../../Libraries/Framework3/include -I../../../Libraries/NSGUI -I../../../Libraries/MeshRenderer2 -I../../../Libraries/tinyxml/include -I../../../Libraries/lua-5.3.4/src -I../../../Libraries/FW3/include -I../../../Libraries/IndieLib/IndieLib/Common -I../../../Libraries/hlsl2glslfork/include -I../../../Libraries/otl/include -I../../../Samples/SimpleTexture -I../../../Libraries/STX/include -I../../../Libraries/Framework3/include -w
CFLAGS += $(PIC3) -fpermissive $(COBJFLAGS)

CXXFLAGS += $(CFLAGS)
CPPFLAGS += $(CFLAGS)

SRCS =  ../../../Samples/TestNoise/TestNoise.cpp


%.o: %.c
	$(GCC) -g3 -O0 $(CFLAGS) -o $@ -c $<

%.o: %.cc
	$(GPP) -g3 -O0 $(NARROWING) -fexceptions -fnon-call-exceptions $(CPLUSPLUS) $(CPPFLAGS) -o $@ -c $<

%.o: %.cpp
	$(GPP) -g3 -O0 $(NARROWING) -fexceptions -fnon-call-exceptions $(CPLUSPLUS) $(CPPFLAGS) -o $@ -c $<

%.o: %.cxx
	$(GPP) -g3 -O0 $(NARROWING) -fexceptions -fnon-call-exceptions $(CPLUSPLUS) $(CXXFLAGS) -o $@ -c $<

ifeq ($(OS), "Darwin")
%.o: %.m
	$(GCC) -g3 -O0 $(COBJFLAGS) -fnested-functions $@ -c $<
else
%.o: %.m
	echo $@ not included in Linux build
endif

ifeq ($(OS), "Darwin")
OBJS=$(patsubst %.m,%.o,$(patsubst %.cpp,%.o,$(patsubst %.cxx,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(filter %.cpp %.cxx %.cc %c %.m ,$(SRCS)))))))
FILES=$(patsubst %.m,%.m~,$(patsubst %.cxx,%.cxx~,$(patsubst %.cpp,%.cpp~,$(patsubst %.cc,%.cc~,$(patsubst %.c,%.c~,$(filter %.c %.cc %.cpp %.cxx %.m,$(SRCS)))))))
else
OBJS=$(patsubst %.cpp,%.o,$(patsubst %.cxx,%.o,$(patsubst %.cc,%.o,$(patsubst %.c,%.o,$(filter %.cpp %.cxx %.cc %c ,$(SRCS))))))
FILES=$(patsubst %.cxx,%.cxx~,$(patsubst %.cpp,%.cpp~,$(patsubst %.cc,%.cc~,$(patsubst %.c,%.c~,$(filter %.c %.cc %.cpp %.cxx ,$(SRCS))))))
endif

ifeq ($(FILEEXT), ".so")
$(TARGET): $(OBJS)
	$(GCC) -shared $(LDFLAGS) -g3 -O0 $(OBJS) -L../../../lib -L../../../Dependencies/vulkan -L/usr/local/lib -lc3dbool -lopentree -lotlutils -lotlmesher -lweberarbaro -lweber -lnova -lIndieLib_Static -lFW3 -lRadeonTerrainDemoLibrary -lNSGUI -lspine_x -lMeshRenderer2 -lFramework -lFramework3 -lImage3 -lSTX -ltinyxml_x -lLua_x -ltiff -ljpeg-8c -lpng-1.5.0 -lhlsl2glsl -lSOIL -lnoise -lassimp-5.0.1_x $(FRAMEWORKS) $(OPENGL) $(SDL) $(CGLIBS) $(PTHREAD) $(X11) -lstdc++ -lz -lm -ldl -o ./lib/lib$(NAME).so
else ifeq ($(FILEEXT), ".a")
$(TARGET): $(OBJS)
	$(AR) -cr $@ $^
	$(RANLIB) $(TARGET)
else
$(TARGET): $(OBJS)
	mkdir -p $(TARGETDIR)
	$(GCC) $(LDFLAGS) -g3 -O0 $(OBJS) -L../../../lib -L../../../Dependencies/vulkan -L/usr/local/lib -lc3dbool -lopentree -lotlutils -lotlmesher -lweberarbaro -lweber -lnova -lIndieLib_Static -lFW3 -lRadeonTerrainDemoLibrary -lNSGUI -lspine_x -lMeshRenderer2 -lFramework -lFramework3 -lImage3 -lSTX -ltinyxml_x -lLua_x -ltiff -ljpeg-8c -lpng-1.5.0 -lhlsl2glsl -lSOIL -lnoise -lassimp-5.0.1_x $(FRAMEWORKS) $(OPENGL) $(SDL) $(CGLIBS) $(PTHREAD) $(X11) -lstdc++ -lz -lm -ldl -o $@
endif

ifeq ($(FILEEXT), ".app")
AppBundle: $(TARGET)

all: AppBundle
	#sudo cp -fr ../../../lib/lib*.a /usr/local/lib
clean:
	$(DEL) $(OBJS) $(FILES) $(TARGET)
	$(DEL) ../../../bin/$(NAME).app

else
all: $(TARGET)
	#sudo cp -fr ../../../lib/lib*.a /usr/local/lib
clean:
	$(DEL) $(OBJS) $(FILES) $(TARGET)
endif
	    