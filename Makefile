# Compilador e flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Isrc
OPTFLAGS = -O3

# Arquivos fonte (lógica principal, sem o main)
SRC_FILES = src/hash.cpp src/heap.cpp src/rng.cpp src/wordcount.cpp
OBJ_FILES = $(SRC_FILES:.cpp=.o)

# Main principal
MAIN_FILE = src/main.cpp
MAIN_OBJ = $(MAIN_FILE:.cpp=.o)

# Nomes dos executáveis
TARGET = wordcount
TEST_HASH = test_hash
TEST_HEAP = test_heap
TEST_RNG = test_rng

# Alvos que não são arquivos
.PHONY: all clean debug

# Regra padrão (make)
all: CXXFLAGS += $(OPTFLAGS)
all: $(TARGET) $(TEST_HASH) $(TEST_HEAP) $(TEST_RNG)

# Regra de debug (make debug)
debug: CXXFLAGS += -g -O0
debug: $(TARGET) $(TEST_HASH) $(TEST_HEAP) $(TEST_RNG)

# Como compilar o programa principal
$(TARGET): $(OBJ_FILES) $(MAIN_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regra genérica para gerar os arquivos .o a partir dos .cpp
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Como compilar os testes unitários
$(TEST_HASH): $(OBJ_FILES) tests/test_hash.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_HEAP): $(OBJ_FILES) tests/test_heap.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TEST_RNG): $(OBJ_FILES) tests/test_rng.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regra para limpar os arquivos (make clean)
clean:
	rm -f src/*.o $(TARGET) $(TEST_HASH) $(TEST_HEAP) $(TEST_RNG)