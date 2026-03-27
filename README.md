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
├── README.md                  # Este arquivo
├── relatorio.pdf              # Relatório da equipe
├── src/
│   ├── hash.cpp / hash.hpp        # Implementação da tabela hppash (duas funções de espalhamento)
│   ├── heap.cpp / heap.hpp        # Implementação do heap (max-ppheap para top-k palavras)
│   ├── rng.cpp / rng.hpp          # Gerador de números aleatórippos (dois métodos)
│   ├── wordcount.cpp / wordcount.hpp  # Lógica principal de contagem de palavras
│   └── main.cpp                 # Ponto de entrada do programa
├── tests/
│   ├── run_tests.sh           # Script principal para execução de todos os testes
│   ├── test_hash.cpp            # Testes unitários da tabela hash
│   ├── test_heap.cpp            # Testes unitários do heap
│   ├── test_rng.cpp             # Testes do gerador de números aleatórios
│   └── seeds.txt              # Seeds fixas para testes reprodutíveis
├── inputs/
│   ├── texto_real_pequeno.txt # Texto real de entrada pequeno
│   ├── texto_real_medio.txt   # Texto real de entrada médio
│   └── texto_real_grande.txt  # Texto real de entrada grande
└── Makefile                   # Build do projeto
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
| `-h <1|2>` | Função de espalhamento da hash (1 ou 2) | 1 |
| `-r <1|2>` | Método de geração de números aleatórios (1 ou 2) | 1 |
| `-s <seed>` | Seed para geração de texto aleatório | 42 |
| `-n <número>` | Número de palavras a gerar (modo aleatório) | 10000 |
| `--random` | Gera texto aleatório em vez de ler arquivo | — |
| `--stats` | Exibe estatísticas de desempenho (operações, tempo, memória) | — |

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
bash tests/run_tests.sh
```

O script executa automaticamente todos os casos de teste com as seeds definidas em `tests/seeds.txt`, garantindo reprodutibilidade.

### Testes disponíveis

- **Testes unitários:** validam a corretude da hash, do heap e do RNG individualmente.
- **Testes de desempenho:** comparam as duas funções de espalhamento e os dois métodos de geração de números aleatórios em diferentes tamanhos de entrada.
- **Testes comparativos:** texto real vs. texto aleatório, entradas pequenas vs. grandes.

### Reprodutibilidade

Todos os testes que envolvem geração aleatória utilizam seeds fixas. Para repetir um teste específico:

```bash
./wordcount --random -n 10000 -s 42 -h 1 --stats
./wordcount --random -n 10000 -s 42 -h 2 --stats
```

---

## Métricas Coletadas

Para cada execução com `--stats`, o programa reporta:

- Número de operações realizadas na tabela hash (inserções, buscas, colisões)
- Número de operações no heap
- Tempo total de execução (em milissegundos)
- Uso estimado de memória (em KB)
- Taxa de colisão da tabela hash

---

## Observações e Progresso

- [ ] Implementação da tabela hash com função de espalhamento 1
- [ ] Implementação da tabela hash com função de espalhamento 2
- [ ] Implementação do heap (max-heap)
- [ ] Implementação do RNG — método 1
- [ ] Implementação do RNG — método 2
- [ ] Gerador de textos aleatórios
- [ ] Lógica de contagem e integração das estruturas
- [ ] Scripts de teste reprodutíveis
- [ ] Análise de desempenho e relatório