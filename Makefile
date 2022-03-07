all:
.SILENT:
PRECMD=echo "  $(@F)" ; mkdir -p $(@D) ;

#TODO Clean this up and make more portable. Experimental for now...

CCINC:=-Isrc -Isrc/libgambatte/include -Isrc/libgambatte/src -Isrc/libgambatte/common -I../ra2/src/emuhost
CCDEF:=-DHAVE_CSTDINT=1
CC:=gcc -c -MMD -O2 $(CCINC) $(CCDEF) -Werror -Wimplicit
CXX:=g++ -c -MMD -O2 $(CCINC) $(CCDEF) -Werror
LD:=g++
LDPOST:=../ra2/out/linux-default/libemuhost.a -lpthread -lz -lpulse-simple -lasound -lGL -lGLESv2 -ldrm -lgbm -lEGL -lX11

CFILES:=$(shell find src -name '*.c' -or -name '*.cpp')
OFILES:=$(patsubst src/%,mid/%.o,$(basename $(CFILES)))
mid/%.o:src/%.c;$(PRECMD) $(CC) -o $@ $<
mid/%.o:src/%.cpp;$(PRECMD) $(CXX) -o $@ $<

-include $(OFILES:.o=.d)

EXE:=out/akgambatte
all:$(EXE)
$(EXE):$(OFILES);$(PRECMD) $(LD) -o $@ $^ $(LDPOST)

clean:;rm -rf mid out

run:$(EXE);$(EXE) ~/rom/gb/z/zelda_seasons.gb
#run:$(EXE);$(EXE) ~/rom/gb/g/gargoyles_quest.gb
