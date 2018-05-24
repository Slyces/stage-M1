PROGRAM := simulation
# Must be the first rule
.PHONY: default
default: $(PROGRAM)

SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:src/%.c=obj/%.o)
DEPENDS := $(SOURCES:src/%.c=deps/%.d)

MAKEFILES := Makefile

CC=gcc
CFLAGS := -O3 -g -std=c99 -Wall -Wno-unused-function
CFLAGS += -DPADAWAN
CFLAGS += -I./include
CFLAGS += $(shell pkg-config igraph --cflags)
LDLIBS := $(shell pkg-config igraph --libs)

LDLIBS += -lpthread

$(OBJECTS): $(MAKEFILES)

$(PROGRAM): $(SOURCES) 
    $(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(OBJECTS): obj/%.o: src/%.c
    $(CC) -o $@ $(CFLAGS) -c $<

.PHONY: depend
depend: $(DEPENDS)

$(DEPENDS): $(MAKEFILES)

$(DEPENDS): deps/%.d: src/%.c
    $(CC) $(CFLAGS) -MM $< | \
                sed -e 's|\(.*\)\.o:|deps/\1.d obj/\1.o:|g' > $@

ifneq ($(MAKECMDGOALS),clean)
-include $(DEPENDS)
endif

.PHONY: clean
clean: 
    rm -f simulation obj/*.o deps/*.d
