CFLAGS := -g -Wall -Wextra -Werror -Wdouble-promotion --pedantic --std=gnu99
CC := gcc
LDFLAGS :=
LDLIBS := -lncurses -lm
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)
prefix := /usr

all: curse
.PHONY: all clean directories install
-include $(addprefix bin/,$(SRCS:.c=.d))

curse: $(addprefix bin/,$(OBJS))
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $@

bin/%.o: %.c | directories
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) -MM -MP -MT $@ $< -o bin/$*.d

directories: bin/

bin/:
	mkdir -p bin

clean:
	rm curse
	rm -rf bin

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
	
indent:
	export VERSION_CONTROL=none; indent $(SRCS) *.h -bli0 -i4 -nut
