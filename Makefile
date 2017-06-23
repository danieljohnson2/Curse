CFLAGS := -g
LDLIBS := -lncurses
SRCS := curse.c perlin.c
OBJS := $(SRCS:.c=.o)

all: bin/curse
.PHONY: all clean directories
-include $(SRCS:.c=.d)

bin/curse: $(addprefix bin/,$(OBJS))
	$(CC) $(LFLAGS) $^ $(LDLIBS) -o $@

bin/%.o: %.c | directories
	$(CC) $(CFLAGS) -c $< -o $@
	$(CC) -MM -MP -MT $@ $< -o bin/$*.d

directories: bin/

bin/:
	mkdir -p bin

clean:
	rm -rf bin
	
indent:
	export VERSION_CONTROL=none; indent $(SRCS) *.h -bli0 -i4 -nut
