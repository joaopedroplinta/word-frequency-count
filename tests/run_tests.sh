#!/bin/bash
# run_tests.sh — executa testes unitários e de desempenho
# Todos os testes aleatórios usam seeds fixas de seeds.txt para reprodutibilidade

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
ROOT="$SCRIPT_DIR/.."
BIN="$ROOT/wordcount"
SEEDS_FILE="$SCRIPT_DIR/seeds.txt"
SEP="============================================================"

GREEN='\033[0;32m'; RED='\033[0;31m'; YELLOW='\033[1;33m'; NC='\033[0m'
pass() { echo -e "  ${GREEN}[PASS]${NC} $1"; }
fail() { echo -e "  ${RED}[FAIL]${NC} $1"; GLOBAL_FAIL=1; }
header() { echo -e "\n${YELLOW}$SEP${NC}\n ${1}\n${YELLOW}$SEP${NC}"; }
GLOBAL_FAIL=0

# Compilar tudo
header "Compilando projeto e testes"
cd "$ROOT"
make all tests -s && pass "Compilação bem-sucedida" || { fail "Falha na compilação"; exit 1; }

# Testes unitários
header "Testes Unitários"
for test_bin in tests/test_rng tests/test_hash tests/test_heap; do
    name=$(basename $test_bin)
    echo -e "\n>> $name"
    if ./$test_bin; then
        pass "$name: todos os casos passaram"
    else
        fail "$name: algum caso falhou"
    fi
done

# Lê seeds do arquivo
SEEDS=()
while IFS= read -r line; do
    [[ "$line" =~ ^#.*$ || -z "$line" ]] && continue
    seed=$(echo $line | awk '{print $1}')
    SEEDS+=("$seed")
done < "$SEEDS_FILE"

# Reprodutibilidade
header "Reprodutibilidade: mesma seed = mesmo resultado"
seed=${SEEDS[0]}
out1=$($BIN --random -n 10000 -s $seed -k 5 -h 1 -r 1 2>/dev/null)
out2=$($BIN --random -n 10000 -s $seed -k 5 -h 1 -r 1 2>/dev/null)
[ "$out1" = "$out2" ] && pass "seed=$seed: LCG+djb2 reprodutível" || fail "seed=$seed: LCG+djb2 diverge"

out3=$($BIN --random -n 10000 -s $seed -k 5 -h 2 -r 2 2>/dev/null)
out4=$($BIN --random -n 10000 -s $seed -k 5 -h 2 -r 2 2>/dev/null)
[ "$out3" = "$out4" ] && pass "seed=$seed: Xorshift+fnv1a reprodutível" || fail "seed=$seed: Xorshift+fnv1a diverge"

out5=$($BIN --random -n 10000 -s ${SEEDS[0]} -k 5 -h 1 2>/dev/null)
out6=$($BIN --random -n 10000 -s ${SEEDS[1]} -k 5 -h 1 2>/dev/null)
[ "$out5" != "$out6" ] && pass "seeds diferentes geram resultados diferentes" || fail "seeds diferentes geraram saídas iguais"

# Escala
header "Testes de Escala"
echo ""
printf "%-12s %-8s %-12s %-12s %-10s\n" "Palavras" "Hash" "Unicas" "Colisoes" "Tempo(ms)"
printf "%-12s %-8s %-12s %-12s %-10s\n" "--------" "----" "------" "--------" "---------"
for n in 1000 10000 100000 500000; do
    for hfunc in 1 2; do
        hname="djb2"; [ $hfunc -eq 2 ] && hname="fnv1a"
        result=$($BIN --random -n $n -s 42 -k 1 -h $hfunc -r 1 --stats 2>/dev/null)
        unique=$(echo "$result" | grep "Palavras" | awk '{print $NF}')
        collis=$(echo "$result" | grep "Colisoes\|Colisões" | awk '{print $NF}')
        tempo=$(echo  "$result" | grep "Tempo" | awk '{print $NF}')
        printf "%-12s %-8s %-12s %-12s %-10s\n" "$n" "$hname" "$unique" "$collis" "$tempo"
    done
done

# Comparação hash
header "Comparação: djb2 vs fnv1a (top-1 por seed)"
echo ""
for seed in "${SEEDS[@]}"; do
    top_djb2=$($BIN  --random -n 50000 -s $seed -k 1 -h 1 2>/dev/null | grep "^1 " | awk '{print $2, $3}')
    top_fnv1a=$($BIN --random -n 50000 -s $seed -k 1 -h 2 2>/dev/null | grep "^1 " | awk '{print $2, $3}')
    printf "  seed=%-8s djb2=[ %-20s ] fnv1a=[ %s ]\n" "$seed" "$top_djb2" "$top_fnv1a"
done

# Comparação RNG
header "Comparação: LCG vs Xorshift (top-3 por seed)"
echo ""
for seed in "${SEEDS[@]}"; do
    top_lcg=$($BIN --random -n 50000 -s $seed -k 3 -r 1 2>/dev/null | grep "^[123] " | awk '{print $2}' | tr '\n' ' ')
    top_xor=$($BIN --random -n 50000 -s $seed -k 3 -r 2 2>/dev/null | grep "^[123] " | awk '{print $2}' | tr '\n' ' ')
    printf "  seed=%-8s LCG=[ %-30s ] Xorshift=[ %s ]\n" "$seed" "$top_lcg" "$top_xor"
done

# Arquivos reais
header "Testes com Arquivos de Texto Real"
for f in "$ROOT/inputs/"*.txt; do
    fname=$(basename "$f")
    echo -e "\n>> $fname"
    for hfunc in 1 2; do
        hname="djb2"; [ $hfunc -eq 2 ] && hname="fnv1a"
        result=$($BIN -f "$f" -k 5 -h $hfunc --stats 2>/dev/null)
        total=$(echo  "$result" | grep "Total"   | awk '{print $NF}')
        unique=$(echo "$result" | grep "nicas"   | awk '{print $NF}')
        lf=$(echo     "$result" | grep "carga"   | awk '{print $NF}')
        tempo=$(echo  "$result" | grep "Tempo"   | awk '{print $NF}')
        printf "  %-8s total=%-8s unicas=%-6s fator_carga=%-8s tempo=%sms\n" "$hname" "$total" "$unique" "$lf" "$tempo"
    done
done

# Resultado
echo ""
echo "$SEP"
if [ $GLOBAL_FAIL -eq 0 ]; then
    echo -e " ${GREEN}TODOS OS TESTES PASSARAM${NC}"
else
    echo -e " ${RED}ALGUNS TESTES FALHARAM${NC}"
fi
echo "$SEP"
exit $GLOBAL_FAIL