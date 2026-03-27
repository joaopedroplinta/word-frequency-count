CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SRCDIR   = src
SRCS     = $(SRCDIR)/main.cpp $(SRCDIR)/hash.cpp $(SRCDIR)/heap.cpp \
           $(SRCDIR)/rng.cpp  $(SRCDIR)/wordcount.cpp
TARGET   = wordcount

.PHONY: all clean debug

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

debug: CXXFLAGS += -g -DDEBUG -O0
debug: $(TARGET)

clean:
	rm -f $(TARGET)