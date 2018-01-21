CFLAGS := -g -Wall -Wextra -Werror -Wdouble-promotion --pedantic --std=gnu99
CC := gcc
LDFLAGS :=
LDLIBS := -lncurses -lm
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
prefix := /usr/local

all: curse
.PHONY: all clean directories install get-dependencies

# Include synthetic makefiles produced from the source;
# we generate these .d files when compiling .c files.
-include $(addprefix bin/,$(SRCS:.c=.d))

curse: $(addprefix bin/,$(OBJS))
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

bin/%.o: %.c | directories
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) -MM -MP -MT $@ $< -o bin/$*.d

directories: bin/

# Build products go in this directory, except for the final executable.
bin/:
	mkdir -p bin

# Removes build products
clean:
	rm curse
	rm -rf bin

# Installs all packages needed to build everything here.
get-dependencies:
	sudo apt-get install -y build-essential libncurses-dev indent

# Installs curse's components; this is also used to construct a .deb
# package.

_share := $(DESTDIR)$(prefix)/share
_games := $(DESTDIR)$(prefix)/games
_doc := $(_share)/doc/curse
_man := $(_share)/man/man6
_icon := $(_share)/icons/hicolor/256x256/apps

install: curse
	install -D curse $(_games)/curse
	install -D desktop/net.verizon.danieljohnson2.curse.desktop -t $(_share)/applications
	install -D desktop/net.verizon.danieljohnson2.curse.png -t $(_icon)
	install -D License.txt $(_doc)/copyright
	install -D README.txt $(_doc)/changelog
	install -D desktop/curse.6 -t $(_man)
	gzip -9 -n -f $(_doc)/changelog
	gzip -9 -n -f $(_man)/curse.6

uninstall:
	rm -f $(_games)/curse
	rm -f $(_share)/applications/net.verizon.danieljohnson2.curse.desktop
	rm -f $(_icon)/net.verizon.danieljohnson2.curse.png
	rm -f $(_doc)/copyright
	rm -f $(_doc)/changelog.gz
	rm -f $(_man)/curse.6.gz

# Pretty-print all the source code
indent:
	export VERSION_CONTROL=none; indent $(SRCS) *.h -bli0 -i4 -nut
