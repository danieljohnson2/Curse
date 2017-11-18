CFLAGS := -g -Wall -Wextra -Werror -Wdouble-promotion --pedantic --std=gnu99
CC := gcc
LDFLAGS :=
LDLIBS := -lncurses -lm
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
prefix := /usr

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
install: curse
	install -d $(prefix)/games
	install -d $(prefix)/share/applications
	install -d $(prefix)/share/icons/hicolor/256x256/apps
	install -d $(prefix)/share/doc/curse
	install -d $(prefix)/share/man/man6
	install curse $(prefix)/games/curse
	install desktop/*.desktop $(prefix)/share/applications
	install desktop/*.png $(prefix)/share/icons/hicolor/256x256/apps
	install License.txt $(prefix)/share/doc/curse/copyright
	install README.txt $(prefix)/share/doc/curse/changelog
	install desktop/curse.6 $(prefix)/share/man/man6
	gzip -9 -n -f $(prefix)/share/doc/curse/changelog
	gzip -9 -n -f $(prefix)/share/man/man6/curse.6

# Pretty-print all the source code
indent:
	export VERSION_CONTROL=none; indent $(SRCS) *.h -bli0 -i4 -nut
