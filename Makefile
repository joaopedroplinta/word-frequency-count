CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
SRCDIR   = src
TESTDIR  = tests

SHARED_SRCS = $(SRCDIR)/hash.cpp $(SRCDIR)/heap.cpp \
              $(SRCDIR)/rng.cpp  $(SRCDIR)/wordcount.cpp

TARGET      = wordcount
TEST_HASH   = $(TESTDIR)/test_hash
TEST_HEAP   = $(TESTDIR)/test_heap
TEST_RNG    = $(TESTDIR)/test_rng
BENCHMARK   = $(TESTDIR)/benchmark

ZIP_NAME = jpshp-omd

.PHONY: all clean debug tests report zip

all: $(TARGET)

$(TARGET): $(SRCDIR)/main.cpp $(SHARED_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_HASH): $(TESTDIR)/test_hash.cpp $(SHARED_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_HEAP): $(TESTDIR)/test_heap.cpp $(SHARED_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_RNG): $(TESTDIR)/test_rng.cpp $(SHARED_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BENCHMARK): $(TESTDIR)/benchmark.cpp $(SHARED_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^

tests: $(TEST_HASH) $(TEST_HEAP) $(TEST_RNG)
	@echo "\n=== Rodando test_rng ==="
	@./$(TEST_RNG)
	@echo "\n=== Rodando test_hash ==="
	@./$(TEST_HASH)
	@echo "\n=== Rodando test_heap ==="
	@./$(TEST_HEAP)

report: $(BENCHMARK)
	@echo "[report] Coletando dados de benchmark..."
	@./$(BENCHMARK) benchmark_results.json
	@echo "[report] Gerando relatorio.tex e relatorio.pdf..."
	@python3 $(TESTDIR)/gen_report.py --json benchmark_results.json --out .
	@echo "[report] Pronto: relatorio.pdf"

debug: CXXFLAGS += -g -DDEBUG -O0
debug: $(TARGET)

clean:
	rm -f $(TARGET) $(TEST_HASH) $(TEST_HEAP) $(TEST_RNG) $(BENCHMARK)
	rm -f benchmark_results.json relatorio.tex relatorio.aux relatorio.log relatorio.toc relatorio.out

zip: relatorio.pdf
	@echo "[zip] Gerando $(ZIP_NAME).zip..."
	@zip -r ../$(ZIP_NAME).zip . \
	    --exclude "*.o" \
	    --exclude "$(TARGET)" \
	    --exclude "$(TEST_HASH)" \
	    --exclude "$(TEST_HEAP)" \
	    --exclude "$(TEST_RNG)" \
	    --exclude "$(BENCHMARK)" \
	    --exclude "benchmark_results.json" \
	    --exclude "relatorio.tex" \
	    --exclude "relatorio.aux" \
	    --exclude "relatorio.log" \
	    --exclude "relatorio.toc" \
	    --exclude "relatorio.out" \
	    --exclude ".git/*" \
	    --exclude ".github/*" \
	    --exclude "CLAUDE.md" \
	    --exclude "*.zip"
	@echo "[zip] Pronto: ../$(ZIP_NAME).zip"