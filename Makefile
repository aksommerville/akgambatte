all:
.SILENT:
PRECMD=echo "  $(@F)" ; mkdir -p $(@D) ;

EHCFG:=../ra3/out/emuhost-config
EH_CFLAGS:=$(shell $(EHCFG) --cflags)
EH_LDFLAGS:=$(shell $(EHCFG) --ldflags)
EH_LIBS:=$(shell $(EHCFG) --libs)
EH_DEPS:=$(shell $(EHCFG) --deps)

CCINC:=-Isrc -Isrc/libgambatte/include -Isrc/libgambatte/src -Isrc/libgambatte/common
CCDEF:=-DHAVE_CSTDINT=1
CC:=gcc -c -MMD -O2 $(CCINC) $(CCDEF) $(EH_CFLAGS) -Werror -Wimplicit
CXX:=g++ -c -MMD -O2 $(CCINC) $(CCDEF) $(EH_CFLAGS) -Werror
LD:=g++ $(EH_LDFLAGS)
LDPOST:=$(EH_LIBS)

CFILES:=$(shell find src -name '*.c' -or -name '*.cpp')
OFILES:=$(patsubst src/%,mid/%.o,$(basename $(CFILES)))
mid/%.o:src/%.c;$(PRECMD) $(CC) -o $@ $<
mid/%.o:src/%.cpp;$(PRECMD) $(CXX) -o $@ $<

-include $(OFILES:.o=.d)

EXE:=out/akgambatte
all:$(EXE)
$(EXE):$(OFILES) $(EH_DEPS);$(PRECMD) $(LD) -o $@ $(OFILES) $(LDPOST)

clean:;rm -rf mid out

run:$(EXE);$(EXE) ~/rom/gb/z/zelda_seasons.gz
#run:$(EXE);$(EXE) ~/rom/gb/g/gargoyles_quest.gb
