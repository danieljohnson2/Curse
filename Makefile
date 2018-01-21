CFLAGS := -g -Wall -Wextra -Werror -Wdouble-promotion --pedantic --std=gnu99
CC := gcc
LDFLAGS :=
LDLIBS := -lncurses -lm
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
DESTDIR :=
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

_share := $(DESTDIR)$(prefix)/share
_games := $(DESTDIR)$(prefix)/games
_doc := $(_share)/doc/curse
_man := $(prefix)/share/man/man6
_icon := $(_share)/icons/hicolor/256x256/apps

# Installs curse's components; this is also used to construct a .deb
# package.
install: curse
	install -d $(_games)
	install -d $(_share)/applications
	install -d $(_icon)
	install -d $(_doc)
	install -d $(_man)
	install curse $(_games)/curse
	install desktop/*.desktop $(_share)/applications
	install desktop/*.png $(_icon)
	install License.txt $(_doc)/copyright
	install README.txt $(_doc)/changelog
	install desktop/curse.6 $(_man)
	gzip -9 -n -f $(_doc)/changelog
	gzip -9 -n -f $(_man)/curse.6

# Pretty-print all the source code
indent:
	export VERSION_CONTROL=none; indent $(SRCS) *.h -bli0 -i4 -nut
