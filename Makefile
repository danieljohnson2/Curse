CFLAGS := -g -Wall -Werror -fmax-errors=3 --pedantic --std=gnu99
CC := gcc
LDFLAGS :=
LDLIBS := -lncurses -lm
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

all: curse
.PHONY: all clean directories
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
	
indent:
	export VERSION_CONTROL=none; indent $(SRCS) *.h -bli0 -i4 -nut
