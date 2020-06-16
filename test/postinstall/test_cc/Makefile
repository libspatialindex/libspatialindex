PROGRAM = test_cc
OBJECTS = test_cc.o

override CXXFLAGS += -std=c++11 -g -Wall -Werror $(shell pkg-config libspatialindex --cflags)
override LDFLAGS += $(shell pkg-config libspatialindex --libs)

all: $(PROGRAM)

$(PROGRAM): $(OBJECTS)
	$(CXX) -o $@ $< $(LDFLAGS)

clean:
	$(RM) $(PROGRAM) $(OBJECTS)

.PHONY: clean
