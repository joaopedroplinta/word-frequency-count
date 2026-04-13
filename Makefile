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
	@PARENT=$$(cd .. && pwd); \
	 rm -rf $$PARENT/$(ZIP_NAME); \
	 mkdir -p $$PARENT/$(ZIP_NAME); \
	 cp -r . $$PARENT/$(ZIP_NAME)/; \
	 rm -rf $$PARENT/$(ZIP_NAME)/.git \
	        $$PARENT/$(ZIP_NAME)/.github \
	        $$PARENT/$(ZIP_NAME)/CLAUDE.md; \
	 rm -f  $$PARENT/$(ZIP_NAME)/$(TARGET) \
	        $$PARENT/$(ZIP_NAME)/$(TEST_HASH) \
	        $$PARENT/$(ZIP_NAME)/$(TEST_HEAP) \
	        $$PARENT/$(ZIP_NAME)/$(TEST_RNG) \
	        $$PARENT/$(ZIP_NAME)/$(BENCHMARK) \
	        $$PARENT/$(ZIP_NAME)/benchmark_results.json \
	        $$PARENT/$(ZIP_NAME)/relatorio.tex \
	        $$PARENT/$(ZIP_NAME)/relatorio.aux \
	        $$PARENT/$(ZIP_NAME)/relatorio.log \
	        $$PARENT/$(ZIP_NAME)/relatorio.toc \
	        $$PARENT/$(ZIP_NAME)/relatorio.out; \
	 find $$PARENT/$(ZIP_NAME) -name "*.zip" -delete; \
	 rm -f $$PARENT/$(ZIP_NAME).zip; \
	 cd $$PARENT && zip -r $(ZIP_NAME).zip $(ZIP_NAME); \
	 rm -rf $$PARENT/$(ZIP_NAME)
	@echo "[zip] Pronto: ../$(ZIP_NAME).zip"
