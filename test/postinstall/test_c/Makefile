PROGRAM = test_c
OBJECTS = test_c.o

override CFLAGS += -g -Wall -Werror $(shell pkg-config libspatialindex --cflags)
override LDFLAGS += $(shell pkg-config libspatialindex --libs)

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CC) -o $@ $< $(LDFLAGS)

clean:
	$(RM) $(PROGRAM) $(OBJECTS)

.PHONY: clean
