PROGRAM := simulation
# Must be the first rule
.PHONY: default
default: $(PROGRAM)

SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:src/%.c=obj/%.o)
DEPENDS := $(SOURCES:src/%.c=deps/%.d)

MAKEFILES := Makefile

CC=gcc
CFLAGS := -O0 -g3 -std=c11 -Wall -Wno-unused-function
CFLAGS += -DPADAWAN
CFLAGS += -I./include
CFLAGS += -I/usr/local/igraph
CFLAGS += $(shell pkg-config igraph --cflags)
LDLIBS := $(shell pkg-config igraph --libs)

LDLIBS += -lpthread

$(OBJECTS): $(MAKEFILES)

$(PROGRAM): $(SOURCES)
	$(CC) $(CFLAGS) $(LDLIBS) -o $@ $^

$(OBJECTS): obj/%.o: src/%.c
	$(CC) -o $@ $(CFLAGS) -c $<

.PHONY: clean
clean: 
	rm -f simulation obj/*.o deps/*.d
