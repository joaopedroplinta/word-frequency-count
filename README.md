# Trabalho 1 — Contagem de Frequência de Palavras

**Disciplina:** Estruturas de Dados Avançadas  
**Curso:** Bacharelado em Ciência da Computação  
**Entrega:** 13 de abril de 2026

## Integrantes

- João Pedro dos Santos Henrique Plinta
- Odair Monteschio Duarte

---

## Descrição do Projeto

Implementação de um sistema de **contagem de frequência de palavras** utilizando de forma integrada:

- **Hash:** tabela hash implementada manualmente para contar a frequência de cada palavra no texto, com suporte a duas funções de espalhamento diferentes.
- **Heap:** heap implementado manualmente para extrair eficientemente as palavras mais frequentes (top-k).
- **Números aleatórios:** gerador de números pseudo-aleatórios implementado manualmente (dois métodos) para geração de textos sintéticos de teste com seeds reprodutíveis.

---

## Estrutura de Arquivos

```
.
├── README.md                          # Este arquivo
├── relatorio.pdf                      # Relatório da equipe
├── .github/workflows/ci.yml          # Pipeline de CI (GitHub Actions)
├── src/
│   ├── hash.cpp / hash.hpp            # Tabela hash (djb2 e FNV-1a)
│   ├── heap.cpp / heap.hpp            # Max-heap para top-k palavras
│   ├── rng.cpp / rng.hpp              # Gerador de números aleatórios (LCG e Xorshift64)
│   ├── wordcount.cpp / wordcount.hpp  # Lógica principal de contagem de palavras
│   └── main.cpp                       # Ponto de entrada do programa
├── tests/
│   ├── run_tests.sh                   # Script principal para execução de todos os testes
│   ├── test_hash.cpp                  # Testes unitários da tabela hash
│   ├── test_heap.cpp                  # Testes unitários do heap
│   ├── test_rng.cpp                   # Testes do gerador de números aleatórios
│   ├── benchmark.cpp                  # Coleta métricas de desempenho e gera benchmark_results.json
│   ├── gen_report.py                  # Lê o JSON e gera relatorio.tex + relatorio.pdf automaticamente
│   └── seeds.txt                      # Seeds fixas para testes reprodutíveis
├── inputs/
│   ├── texto_real_pequeno.txt         # Texto de entrada pequeno (~500 palavras)
│   ├── texto_real_medio.txt           # Texto de entrada médio (~5000 palavras)
│   └── texto_real_grande.txt          # Texto de entrada grande (~50000 palavras)
└── Makefile                           # Build do projeto
```

---

## Compilação

```bash
make
```

Para compilar em modo debug:

```bash
make debug
```

Para limpar os arquivos compilados:

```bash
make clean
```

---

## Execução

### Uso básico

```bash
./wordcount [opções]
```

### Opções disponíveis

| Opção | Descrição | Padrão |
|---|---|---|
| `-f <arquivo>` | Arquivo de texto de entrada | stdin |
| `-k <número>` | Quantidade de palavras mais frequentes a exibir | 10 |
| `-h <1\|2>` | Função de espalhamento da hash: 1=djb2, 2=FNV-1a | 1 |
| `-r <1\|2>` | Método de geração aleatória: 1=LCG, 2=Xorshift64 | 1 |
| `-s <seed>` | Seed para geração de texto aleatório | 42 |
| `-n <número>` | Número de palavras a gerar (modo aleatório) | 10000 |
| `-c <número>` | Capacidade inicial da tabela hash (>= 1) | 16384 |
| `--random` | Gera texto aleatório em vez de ler arquivo | — |
| `--stats` | Exibe estatísticas de desempenho | — |

### Exemplos

```bash
# Contar frequência a partir de um arquivo real, exibir top 20
./wordcount -f inputs/texto_real_grande.txt -k 20 --stats

# Gerar texto aleatório com seed fixa e contar frequência
./wordcount --random -n 50000 -s 1234 -k 10 --stats

# Usar função de espalhamento 2 com texto real
./wordcount -f inputs/texto_real_medio.txt -h 2 -k 15 --stats
```

---

## Testes

### Executar todos os testes

```bash
make tests
```

Ou via script de integração completo (inclui testes de escala e comparativos):

```bash
bash tests/run_tests.sh
```

### Testes disponíveis

- **Testes unitários:** validam a corretude da hash, do heap e do RNG individualmente (100 casos no total), incluindo verificação exaustiva palavra por palavra contra `std::unordered_map` nos três arquivos reais e testes de validação de entrada.
- **Testes de escala:** comparam djb2 e FNV-1a em entradas de 1.000 a 500.000 palavras.
- **Testes comparativos:** LCG vs Xorshift64, texto real vs texto aleatório.

### Reprodutibilidade

Todos os testes que envolvem geração aleatória utilizam seeds fixas. Para repetir um teste específico:

```bash
./wordcount --random -n 10000 -s 42 -h 1 --stats
./wordcount --random -n 10000 -s 42 -h 2 --stats
```

---

## Geração do Relatório

O relatório PDF é gerado automaticamente a partir dos dados reais de benchmark:

```bash
make report
```

Esse comando compila o benchmark, coleta as métricas, gera `relatorio.tex`
com os dados reais preenchidos e compila o `relatorio.pdf` via `pdflatex`.
Requer Python 3 e `pdflatex` (TeX Live) instalados.

---

## Métricas Coletadas

Para cada execução com `--stats`, o programa reporta:

- Número de inserções e colisões na tabela hash
- Número de rehashes realizados e capacidade final da tabela
- Fator de carga da tabela hash
- Número de operações no heap
- Tempo total de execução (em milissegundos)
- Uso de memória RSS do processo (em KB)

---

## Observações e Progresso

- [x] Implementação da tabela hash com função de espalhamento 1 (djb2)
- [x] Implementação da tabela hash com função de espalhamento 2 (FNV-1a)
- [x] Implementação do heap (max-heap)
- [x] Implementação do RNG — método 1 (LCG)
- [x] Implementação do RNG — método 2 (Xorshift64)
- [x] Gerador de textos aleatórios
- [x] Lógica de contagem e integração das estruturas
- [x] Scripts de teste reprodutíveis
- [x] Análise de desempenho e relatório
- [x] Validação de entradas inválidas (`-c 0`, `RNG::next_in(0)`)
- [x] Extração top-k sem cópia desnecessária do heap (O(n + k log n))
- [x] Comentários de complexidade nos métodos críticos
- [x] Pipeline de CI com GitHub Actions