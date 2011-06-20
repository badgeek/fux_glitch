# change to your local directories!
PD_APP_DIR = /Applications/Pd-extended.app/Contents/Resources
PD_DIR = /Users/xcorex/Documents/Documents/Projects/Puredata/PdSource/Pd-0.42.5-extended/pd
GEM_DIR = /Users/xcorex/Documents/Documents/Projects/Puredata/PdSource/Pd-0.42.5-extended/Gem
# build flags

INCLUDES = -I$(PD_DIR)/include
CPPFLAGS = -g -O2 -fPIC -freg-struct-return -Os -falign-loops=32 -falign-functions=32 -falign-jumps=32 -funroll-loops -ffast-math  -arch i386 -mmmx -fpascal-strings 

UNAME := $(shell uname -s)
ifeq ($(UNAME),Linux)
 CPPFLAGS += -DLINUX
 INCLUDES += `pkg-config --cflags libfreenect`
 LDFLAGS =  -export_dynamic -shared
 LIBS = `pkg-config --libs libfreenect`
 EXTENSION = pd_linux
endif
ifeq ($(UNAME),Darwin)
 CPPFLAGS += -DDARWIN
 INCLUDES +=  -I/sw/include -I/sw/include/FTGL -I/sw/include/freetype2 -I$(GEM_DIR)/src -I/usr/include/FTGL -I/usr/include/freetype2 -I$(PD_DIR)/src -I$(PD_DIR) 
 LDFLAGS = -c -arch i386 
 LIBS =  -lm
 EXTENSION = pd_darwin
endif

.SUFFIXES = $(EXTENSION)

SOURCES = fux_glitch
#OUTPUT  = mesh_square

all:
	g++ $(LDFLAGS) $(INCLUDES) $(CPPFLAGS) -o $(SOURCES).o -c $(SOURCES).cpp
	g++ -o $(SOURCES).$(EXTENSION) -arch i386 -dynamiclib -mmacosx-version-min=10.3 -undefined dynamic_lookup -framework QuickTime -framework Carbon -framework AGL -framework OpenGL -arch i386 ./*.o -L/sw/lib -lfreeimage -lftgl  -lstdc++ -ldl -lz -lm -lpthread -L$(PD_DIR)/bin -L$(PD_DIR)
	rm -fr ./*.o
deploy:
	rm -fr $(PD_APP_DIR)/extra/$(SOURCES).$(EXTENSION)
	mv *.$(EXTENSION) $(PD_APP_DIR)/extra/
clean:
	rm -f $(SOURCES)*.o
	rm -f $(SOURCES)*.$(EXTENSION)
distro: clean all
	rm *.o
