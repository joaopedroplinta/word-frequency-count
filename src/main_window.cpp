#include "main_window.hpp"
#include "wordcount.hpp"
#include "rng.hpp"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QSplitter>
#include <QScrollArea>
#include <QFileDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <fstream>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setWindowTitle("WordCount — Frequência de Palavras");
    resize(960, 640);
    setupUI();
}

void MainWindow::setupUI() {
    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    setCentralWidget(splitter);

    // ============================================================
    // PAINEL ESQUERDO — Configurações
    // ============================================================
    QScrollArea* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setMinimumWidth(280);
    scroll->setMaximumWidth(320);

    QWidget* left = new QWidget();
    QVBoxLayout* vl = new QVBoxLayout(left);
    vl->setSpacing(8);

    // --- Fonte de dados ---
    QGroupBox* grp_src = new QGroupBox("Fonte de dados");
    QVBoxLayout* src_vl = new QVBoxLayout(grp_src);

    rb_file_   = new QRadioButton("Arquivo");
    rb_random_ = new QRadioButton("Aleatório");
    rb_file_->setChecked(true);
    src_vl->addWidget(rb_file_);
    src_vl->addWidget(rb_random_);

    QHBoxLayout* file_hl = new QHBoxLayout();
    file_path_  = new QLineEdit();
    file_path_->setPlaceholderText("Selecione um arquivo...");
    btn_browse_ = new QPushButton("...");
    btn_browse_->setFixedWidth(32);
    file_hl->addWidget(file_path_);
    file_hl->addWidget(btn_browse_);
    src_vl->addLayout(file_hl);

    vl->addWidget(grp_src);

    // --- Parâmetros aleatórios (desabilitado por padrão) ---
    grp_random_ = new QGroupBox("Parâmetros aleatórios");
    grp_random_->setEnabled(false);
    QFormLayout* rnd_fl = new QFormLayout(grp_random_);
    spin_seed_   = new QSpinBox();
    spin_seed_->setRange(0, 2147483647);
    spin_seed_->setValue(42);
    spin_nwords_ = new QSpinBox();
    spin_nwords_->setRange(1, 10000000);
    spin_nwords_->setValue(10000);
    spin_nwords_->setSingleStep(1000);
    rnd_fl->addRow("Seed:", spin_seed_);
    rnd_fl->addRow("Nº de palavras:", spin_nwords_);

    vl->addWidget(grp_random_);

    // --- Consulta ---
    QGroupBox* grp_q = new QGroupBox("Consulta");
    QFormLayout* q_fl = new QFormLayout(grp_q);

    spin_k_ = new QSpinBox();
    spin_k_->setRange(1, 100000);
    spin_k_->setValue(10);

    spin_offset_ = new QSpinBox();
    spin_offset_->setRange(0, 100000);
    spin_offset_->setValue(0);
    spin_offset_->setToolTip("Pular as primeiras N posições.\nEx: k=10, offset=10 → posições 11-20");

    rb_top_    = new QRadioButton("Mais frequentes");
    rb_bottom_ = new QRadioButton("Menos frequentes");
    rb_top_->setChecked(true);
    QHBoxLayout* freq_hl = new QHBoxLayout();
    freq_hl->setContentsMargins(0, 0, 0, 0);
    freq_hl->addWidget(rb_top_);
    freq_hl->addWidget(rb_bottom_);
    QWidget* freq_w = new QWidget();
    freq_w->setLayout(freq_hl);

    q_fl->addRow("k (quantidade):", spin_k_);
    q_fl->addRow("Offset:", spin_offset_);
    q_fl->addRow("Frequência:", freq_w);

    vl->addWidget(grp_q);

    // --- Hash ---
    QGroupBox* grp_hash = new QGroupBox("Função de Hash");
    QFormLayout* hash_fl = new QFormLayout(grp_hash);

    rb_djb2_  = new QRadioButton("djb2");
    rb_fnv1a_ = new QRadioButton("FNV-1a");
    rb_djb2_->setChecked(true);
    QHBoxLayout* hash_rbs = new QHBoxLayout();
    hash_rbs->setContentsMargins(0, 0, 0, 0);
    hash_rbs->addWidget(rb_djb2_);
    hash_rbs->addWidget(rb_fnv1a_);
    QWidget* hash_rbs_w = new QWidget();
    hash_rbs_w->setLayout(hash_rbs);

    edit_capacity_ = new QLineEdit("16384");
    edit_capacity_->setToolTip("Capacidade inicial da tabela hash (potência de 2 recomendada)");

    hash_fl->addRow("Função:", hash_rbs_w);
    hash_fl->addRow("Capacidade inicial:", edit_capacity_);

    vl->addWidget(grp_hash);

    // --- RNG ---
    QGroupBox* grp_rng = new QGroupBox("Gerador aleatório (RNG)");
    QVBoxLayout* rng_vl = new QVBoxLayout(grp_rng);
    rb_lcg_      = new QRadioButton("LCG");
    rb_xorshift_ = new QRadioButton("Xorshift64");
    rb_lcg_->setChecked(true);
    rng_vl->addWidget(rb_lcg_);
    rng_vl->addWidget(rb_xorshift_);

    vl->addWidget(grp_rng);

    // --- Botão Executar ---
    btn_run_ = new QPushButton("Executar");
    btn_run_->setMinimumHeight(38);
    QFont f = btn_run_->font();
    f.setBold(true);
    btn_run_->setFont(f);
    vl->addWidget(btn_run_);
    vl->addStretch();

    scroll->setWidget(left);
    splitter->addWidget(scroll);

    // ============================================================
    // PAINEL DIREITO — Resultados + Estatísticas
    // ============================================================
    QWidget* right = new QWidget();
    QVBoxLayout* rl = new QVBoxLayout(right);

    // Tabela de resultados
    grp_results_ = new QGroupBox("Resultados");
    QVBoxLayout* res_vl = new QVBoxLayout(grp_results_);
    table_ = new QTableWidget(0, 3);
    table_->setHorizontalHeaderLabels({"#", "Palavra", "Frequência"});
    table_->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    table_->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setAlternatingRowColors(true);
    res_vl->addWidget(table_);
    rl->addWidget(grp_results_, 2);

    // Estatísticas
    QGroupBox* grp_stats = new QGroupBox("Estatísticas");
    QFormLayout* stats_fl = new QFormLayout(grp_stats);
    stats_fl->setHorizontalSpacing(24);

    stat_total_      = new QLabel("—"); stats_fl->addRow("Total de palavras:",   stat_total_);
    stat_unique_     = new QLabel("—"); stats_fl->addRow("Palavras únicas:",     stat_unique_);
    stat_inserts_    = new QLabel("—"); stats_fl->addRow("Inserções na hash:",   stat_inserts_);
    stat_collisions_ = new QLabel("—"); stats_fl->addRow("Colisões na hash:",    stat_collisions_);
    stat_rehashes_   = new QLabel("—"); stats_fl->addRow("Rehashes:",            stat_rehashes_);
    stat_capacity_   = new QLabel("—"); stats_fl->addRow("Capacidade final:",    stat_capacity_);
    stat_load_       = new QLabel("—"); stats_fl->addRow("Fator de carga:",      stat_load_);
    stat_heap_ops_   = new QLabel("—"); stats_fl->addRow("Operações no heap:",   stat_heap_ops_);
    stat_time_       = new QLabel("—"); stats_fl->addRow("Tempo total (ms):",    stat_time_);
    stat_memory_     = new QLabel("—"); stats_fl->addRow("Memória RSS (KB):",    stat_memory_);

    rl->addWidget(grp_stats, 1);

    splitter->addWidget(right);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);

    // Conexões
    connect(btn_browse_, &QPushButton::clicked,  this, &MainWindow::browseFile);
    connect(btn_run_,    &QPushButton::clicked,  this, &MainWindow::runAnalysis);
    connect(rb_file_,    &QRadioButton::toggled, this, &MainWindow::onModeChanged);
    connect(rb_random_,  &QRadioButton::toggled, this, &MainWindow::onModeChanged);
}

void MainWindow::browseFile() {
    QString path = QFileDialog::getOpenFileName(
        this, "Abrir arquivo de texto", QString(),
        "Arquivos de texto (*.txt);;Todos os arquivos (*)");
    if (!path.isEmpty())
        file_path_->setText(path);
}

void MainWindow::onModeChanged() {
    bool is_random = rb_random_->isChecked();
    grp_random_->setEnabled(is_random);
    file_path_->setEnabled(!is_random);
    btn_browse_->setEnabled(!is_random);
}

void MainWindow::runAnalysis() {
    bool ok;
    size_t capacity = edit_capacity_->text().toULongLong(&ok);
    if (!ok || capacity == 0) {
        QMessageBox::warning(this, "Erro", "Capacidade inválida. Informe um inteiro positivo.");
        return;
    }

    HashFunc    hf = rb_fnv1a_->isChecked()    ? HashFunc::FNV1A        : HashFunc::DJB2;
    RNG::Method rm = rb_xorshift_->isChecked() ? RNG::Method::XORSHIFT  : RNG::Method::LCG;
    size_t k       = static_cast<size_t>(spin_k_->value());
    size_t offset  = static_cast<size_t>(spin_offset_->value());
    bool   bottom  = rb_bottom_->isChecked();

    WordCounter wc(capacity, hf);

    try {
        if (rb_random_->isChecked()) {
            uint64_t seed  = static_cast<uint64_t>(spin_seed_->value());
            size_t   nwords = static_cast<size_t>(spin_nwords_->value());
            RNG rng(seed, rm);
            wc.count_from_random(nwords, rng);
        } else {
            QString path = file_path_->text().trimmed();
            if (path.isEmpty()) {
                QMessageBox::warning(this, "Erro", "Selecione um arquivo de entrada.");
                return;
            }
            std::ifstream file(path.toStdString());
            if (!file) {
                QMessageBox::critical(this, "Erro ao abrir arquivo",
                                      "Não foi possível abrir:\n" + path);
                return;
            }
            wc.count_from_stream(file);
        }
    } catch (const std::exception& e) {
        QMessageBox::critical(this, "Erro na análise", QString(e.what()));
        return;
    }

    auto results = bottom ? wc.bottom_k(k, offset) : wc.top_k(k, offset);

    // Atualiza título do grupo resultado
    const char* label = bottom ? "menos" : "mais";
    if (offset > 0)
        grp_results_->setTitle(QString("Resultados — posições %1–%2 (%3 frequentes)")
            .arg(offset + 1).arg(offset + k).arg(label));
    else
        grp_results_->setTitle(QString("Resultados — top %1 %2 frequentes")
            .arg(k).arg(label));

    // Preenche tabela
    table_->setRowCount(static_cast<int>(results.size()));
    for (int i = 0; i < static_cast<int>(results.size()); ++i) {
        auto* rank  = new QTableWidgetItem(QString::number(offset + i + 1));
        auto* word  = new QTableWidgetItem(QString::fromStdString(results[i].word));
        auto* count = new QTableWidgetItem(QString::number(results[i].count));
        rank->setTextAlignment(Qt::AlignCenter);
        count->setTextAlignment(Qt::AlignCenter);
        table_->setItem(i, 0, rank);
        table_->setItem(i, 1, word);
        table_->setItem(i, 2, count);
    }

    showStats(wc.stats());
}

void MainWindow::showStats(const RunStats& stats) {
    stat_total_->setText(QString::number(stats.total_words));
    stat_unique_->setText(QString::number(stats.unique_words));
    stat_inserts_->setText(QString::number(stats.hash_inserts));
    stat_collisions_->setText(QString::number(stats.hash_collisions));
    stat_rehashes_->setText(QString::number(stats.hash_rehashes));
    stat_capacity_->setText(QString::number(stats.hash_capacity));
    stat_load_->setText(QString::number(stats.load_factor, 'f', 4));
    stat_heap_ops_->setText(QString::number(stats.heap_ops));
    stat_time_->setText(QString::number(stats.time_ms));
    stat_memory_->setText(QString::number(stats.memory_kb));
}
