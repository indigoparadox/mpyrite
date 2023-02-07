
# vim: ft=make noexpandtab

C_FILES := src/main.c src/parser.c src/astree.c src/interp.c src/callback.c

include maug/Makefile.inc

# Target-specific options.
.PHONY: clean

all: mpyrit.ale mpyrit.sdl mpyritd.exe mpyritw.exe mpyritnt.exe mpyrit.html

CHECK_C_FILES=check/check.c check/ckparser.c src/parser.c src/astree.c check/ckastree.c src/interp.c

mpyrit.check: $(addprefix obj/check/,$(subst .c,.o,$(CHECK_C_FILES)))
	gcc -g -o $@ $^ $(shell pkg-config check --libs)

obj/check/%.o: %.c
	mkdir -p $(dir $@)
	gcc -g -c $< -o $@ -Isrc/ -Imaug/src/ $(shell pkg-config check --cflags) -DDEBUG_LOG -DDEBUG -DDEBUG_THRESHOLD=1

# Unix (Allegro)

$(eval $(call TGTUNIXALE,mpyrit))

# Unix (SDL)

$(eval $(call TGTUNIXSDL,mpyrit))

# WASM

$(eval $(call TGTWASMSDL,mpyrit))

# DOS

$(eval $(call TGTDOSALE,mpyrit))

# WinNT

$(eval $(call TGTWINNT,mpyrit))

# Win386

$(eval $(call TGTWIN386,mpyrit))

# Clean

clean:
	rm -rf $(CLEAN_TARGETS)

