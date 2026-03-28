CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SRCDIR   = src
TESTDIR  = tests

# Fontes compartilhadas (sem main.cpp)
SHARED_SRCS = $(SRCDIR)/hash.cpp $(SRCDIR)/heap.cpp \
              $(SRCDIR)/rng.cpp  $(SRCDIR)/wordcount.cpp

TARGET       = wordcount
TEST_HASH    = $(TESTDIR)/test_hash
TEST_HEAP    = $(TESTDIR)/test_heap
TEST_RNG     = $(TESTDIR)/test_rng

.PHONY: all clean debug tests

all: $(TARGET)

$(TARGET): $(SRCDIR)/main.cpp $(SHARED_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_HASH): $(TESTDIR)/test_hash.cpp $(SHARED_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_HEAP): $(TESTDIR)/test_heap.cpp $(SHARED_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_RNG): $(TESTDIR)/test_rng.cpp $(SHARED_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

tests: $(TEST_HASH) $(TEST_HEAP) $(TEST_RNG)
	@echo "\n=== Rodando test_rng ==="
	@./$(TEST_RNG)
	@echo "\n=== Rodando test_hash ==="
	@./$(TEST_HASH)
	@echo "\n=== Rodando test_heap ==="
	@./$(TEST_HEAP)

debug: CXXFLAGS += -g -DDEBUG -O0
debug: $(TARGET)

clean:
	rm -f $(TARGET) $(TEST_HASH) $(TEST_HEAP) $(TEST_RNG)