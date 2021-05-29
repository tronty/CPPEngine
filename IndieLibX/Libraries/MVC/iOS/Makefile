OS="Windows"
STR = $(shell uname -a)
ifneq (, $(findstring Ubuntu, $(STR)))
	OS="Ubuntu"
else ifneq (, $(findstring Darwin, $(STR)))
	OS="Darwin"
else ifneq (, $(findstring Debian, $(STR)))
	OS="Debian"
else
	OS="Windows"
endif

SIMULATE ?= yes

ifeq ($(SIMULATE), yes)
DEV := /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneSimulator.platform/Developer
BIN := $(DEV)/usr/bin
else
DEV := /Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer
BIN := $(DEV)/usr/bin
endif

ifeq ($(SIMULATE), yes)

CC := $(BIN)/gcc
CFLAGS += -arch i386
LDFLAGS += -arch i386
CFLAGS += -isysroot $(DEV)/SDKs/iPhoneSimulator6.0.sdk

else

CC := $(BIN)/gcc
CFLAGS += -arch armv6 -mthumb 
LDFLAGS += -arch armv6 -mthumb 
CFLAGS += -isysroot $(DEV)/SDKs/iPhoneOS6.0.sdk

endif

MFLAGS += -fobjc-abi-version=2
MFLAGS += -fobjc-legacy-dispatch
CFLAGS += -I.
CFLAGS += -mmacosx-version-min=10.6
CFLAGS += -D__IPHONE_OS_VERSION_MIN_REQUIRED=40100
CFLAGS += -Wno-import #-Werror
LDFLAGS += -Xlinker -objc_abi_version -Xlinker 2
LDFLAGS += -lobjc -lstdc++
LDFLAGS += -framework Foundation -framework UIKit -framework CoreGraphics -framework OpenGLES -framework CoreAudio -framework AudioToolbox -framework QuartzCore
MFLAGS += $(CFLAGS)
CPPFLAGS += $(CFLAGS)

ifneq ("Darwin",$(OS))
CPPFLAGS += -std=gnu++0x
endif

TARGET := main
OBJS := main.o \
	EAGLView.o \
	Lesson01AppDelegate.o \
	Lesson01.o \
	Lesson.o

all: $(TARGET)
clean:
	rm -fr *.o $(TARGET) $(TARGET).app

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^
	$(DEV)/Applications/iPhone\ Simulator.app/Contents/MacOS/iPhone\ Simulator \ -SimulateApplication ./$(TARGET)

%.o: %.cpp
	$(CC) $(CPPFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.m
	$(CC) $(MFLAGS) -c -o $@ $<

%.o: %.mm
	$(CC) $(MFLAGS) -c -o $@ $<

CODESIGN_ALLOCATE = $(BIN)/codesign_allocate

GEN_UUID := EA72E90C-C91B-11DF-8FC4-539D12B3A9FF
## $(uuid | tr '[:lower:]' '[:upper:]')

AppBundle: code-sign
	mkdir -p $(TARGET).app
	cp Lesson01-Info.plist $(TARGET).app/Info.plist
	touch $(TARGET).app/PkgInfo
	cp $(TARGET) $(TARGET).app/

code-sign: $(TARGET)
#	codesign -f -s "iPhone Developer" $(TARGET)

install2sim: AppBundle
	mkdir -p ~/Library/Application\ Support/iPhone\ Simulator/6.0/Applications/$(GEN_UUID)
	cp -rf $(TARGET).app ~/Library/Application\ Support/iPhone\ Simulator/6.0/Applications/$(GEN_UUID)/

run_sim: install2sim
	open -a "iPhone Simulator"

