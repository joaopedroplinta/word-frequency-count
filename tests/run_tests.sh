#!/bin/bash

# Cores
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo -e "${BLUE}====================================================${NC}"
echo -e "${BLUE}  Iniciando Testes e Geração Automática do LaTeX... ${NC}"
echo -e "${BLUE}====================================================\n${NC}"

# 1. Compila
make clean && make
if [ $? -ne 0 ]; then
    echo "Erro na compilação!"
    exit 1
fi

ARQ="inputs/texto_real_grande.txt"
if [ ! -f "$ARQ" ]; then
    echo "Aviso: Arquivo $ARQ não encontrado! O relatório terá valores vazios."
fi

echo -e "${YELLOW}[1/3] Coletando dados da Tabela Hash (DJB2 vs FNV1A)...${NC}"
# Roda DJB2 e captura os valores
OUT_DJB2=$(./wordcount -f "$ARQ" -h 1 --stats)
TIME_DJB2=$(echo "$OUT_DJB2" | grep "Tempo total" | awk '{print $NF}')
COL_DJB2=$(echo "$OUT_DJB2" | grep "Colisões na hash" | awk '{print $NF}')
INS_DJB2=$(echo "$OUT_DJB2" | grep "Inserções na hash" | awk '{print $NF}')
FC_DJB2=$(echo "$OUT_DJB2" | grep "Fator de carga" | awk '{print $NF}')

# Roda FNV1A e captura os valores
OUT_FNV=$(./wordcount -f "$ARQ" -h 2 --stats)
TIME_FNV=$(echo "$OUT_FNV" | grep "Tempo total" | awk '{print $NF}')
COL_FNV=$(echo "$OUT_FNV" | grep "Colisões na hash" | awk '{print $NF}')
INS_FNV=$(echo "$OUT_FNV" | grep "Inserções na hash" | awk '{print $NF}')
FC_FNV=$(echo "$OUT_FNV" | grep "Fator de carga" | awk '{print $NF}')

echo -e "${YELLOW}[2/3] Coletando dados dos Geradores (LCG vs XORSHIFT)...${NC}"
OUT_LCG=$(./wordcount --random -n 50000 -s 42 -r 1 -h 1 --stats)
TIME_LCG=$(echo "$OUT_LCG" | grep "Tempo total" | awk '{print $NF}')
OPS_LCG=$(echo "$OUT_LCG" | grep "Inserções na hash" | awk '{print $NF}')

OUT_XOR=$(./wordcount --random -n 50000 -s 42 -r 2 -h 1 --stats)
TIME_XOR=$(echo "$OUT_XOR" | grep "Tempo total" | awk '{print $NF}')
OPS_XOR=$(echo "$OUT_XOR" | grep "Inserções na hash" | awk '{print $NF}')

echo -e "${YELLOW}[3/3] Escrevendo relatorio.tex...${NC}"

# Cria o template LaTeX bloqueando expansão do bash (usando 'EOF')
cat << 'EOF' > relatorio.tex
\documentclass[12pt,a4paper]{article}
\usepackage[utf8]{inputenc}
\usepackage[T1]{fontenc}
\usepackage[portuguese]{babel}
\usepackage{geometry}
\usepackage{booktabs}
\usepackage{hyperref}
\usepackage{float}

\geometry{a4paper, margin=2.5cm}

\title{\textbf{Trabalho 1 — Contagem de Frequência de Palavras} \\ \large Estruturas de Dados Avançadas}
\author{João Pedro dos Santos Henrique Plinta \\ Odair Monteschio Duarte}
\date{\today}

\begin{document}
\maketitle

\begin{abstract}
Este relatório apresenta a análise empírica de um sistema de contagem de frequência de palavras desenvolvido em C++, gerado de forma totalmente automatizada pelos scripts de teste da aplicação.
\end{abstract}

\section{Análise das Funções de Espalhamento}
A tabela abaixo compara o desempenho das funções DJB2 e FNV-1a utilizando o arquivo de texto grande, evidenciando o compromisso entre tempo de execução e taxa de colisão.

\begin{table}[H]
\centering
\caption{Comparativo de desempenho entre DJB2 e FNV-1a (Texto Grande)}
\begin{tabular}{@{}lcccc@{}}
\toprule
\textbf{Função} & \textbf{Tempo (ms)} & \textbf{Colisões} & \textbf{Inserções} & \textbf{Fator de Carga} \\ \midrule
DJB2            & __TIME_DJB2__       & __COL_DJB2__      & __INS_DJB2__       & __FC_DJB2__             \\
FNV-1a          & __TIME_FNV__        & __COL_FNV__       & __INS_FNV__        & __FC_FNV__              \\ \bottomrule
\end{tabular}
\end{table}

\section{Análise dos Geradores Aleatórios (RNG)}
Para validar a robustez, 50.000 palavras sintéticas foram geradas usando uma seed estática (42).

\begin{table}[H]
\centering
\caption{Desempenho da geração sintética (50.000 palavras)}
\begin{tabular}{@{}lcc@{}}
\toprule
\textbf{Método RNG} & \textbf{Tempo Total (ms)} & \textbf{Operações} \\ \midrule
LCG                 & __TIME_LCG__              & __OPS_LCG__        \\
Xorshift (64-bit)   & __TIME_XOR__              & __OPS_XOR__        \\ \bottomrule
\end{tabular}
\end{table}

\section{Conclusão}
O sistema demonstrou alta estabilidade e eficiência, comprovadas pela extração automatizada de métricas. As estruturas clássicas como a tabela hash e o heap garantem processamento em escala mantendo tempo hábil.

\end{document}
EOF

# Injeta as variáveis do bash para dentro do arquivo .tex substituindo as tags
sed -i "s/__TIME_DJB2__/$TIME_DJB2/g" relatorio.tex
sed -i "s/__COL_DJB2__/$COL_DJB2/g" relatorio.tex
sed -i "s/__INS_DJB2__/$INS_DJB2/g" relatorio.tex
sed -i "s/__FC_DJB2__/$FC_DJB2/g" relatorio.tex

sed -i "s/__TIME_FNV__/$TIME_FNV/g" relatorio.tex
sed -i "s/__COL_FNV__/$COL_FNV/g" relatorio.tex
sed -i "s/__INS_FNV__/$INS_FNV/g" relatorio.tex
sed -i "s/__FC_FNV__/$FC_FNV/g" relatorio.tex

sed -i "s/__TIME_LCG__/$TIME_LCG/g" relatorio.tex
sed -i "s/__OPS_LCG__/$OPS_LCG/g" relatorio.tex
sed -i "s/__TIME_XOR__/$TIME_XOR/g" relatorio.tex
sed -i "s/__OPS_XOR__/$OPS_XOR/g" relatorio.tex

echo -e "${GREEN}✅ Sucesso! O arquivo 'relatorio.tex' foi gerado na raiz do projeto com os dados reais da sua máquina.${NC}"
echo -e "${BLUE}====================================================${NC}"