
# vim: ft=make noexpandtab

C_FILES := src/main.c src/parser.c src/astree.c

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: mpy.ale mpy.sdl mpyd.exe mpyw.exe mpynt.exe mpy.html

CHECK_C_FILES=check/check.c check/ckparser.c src/parser.c src/astree.c

mpy.check: $(addprefix obj/check/,$(subst .c,.o,$(CHECK_C_FILES)))
	gcc -o $@ $^ $(shell pkg-config check --libs)

obj/check/%.o: %.c
	mkdir -p $(dir $@)
	gcc -c $< -o $@ -Isrc/ -Imaug/src/ $(shell pkg-config check --cflags)

# Unix (Allegro)

$(eval $(call TGTUNIXALE,mpy))

# Unix (SDL)

$(eval $(call TGTUNIXSDL,mpy))

# WASM

$(eval $(call TGTWASMSDL,mpy))

# DOS

$(eval $(call TGTDOSALE,mpy))

# WinNT

$(eval $(call TGTWINNT,mpy))

# Win386

$(eval $(call TGTWIN386,mpy))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

