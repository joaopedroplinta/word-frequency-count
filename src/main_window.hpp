#pragma once
#include <QMainWindow>
#include <QTableWidget>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QGroupBox>
#include <QPushButton>
#include <QSpinBox>
#include "wordcount.hpp"

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);

private slots:
    void browseFile();
    void runAnalysis();
    void onModeChanged();

private:
    void setupUI();
    void showStats(const RunStats& stats);

    // Fonte
    QRadioButton* rb_file_;
    QRadioButton* rb_random_;
    QLineEdit*    file_path_;
    QPushButton*  btn_browse_;
    QGroupBox*    grp_random_;
    QSpinBox*     spin_seed_;
    QSpinBox*     spin_nwords_;

    // Consulta
    QSpinBox*     spin_k_;
    QSpinBox*     spin_offset_;
    QRadioButton* rb_top_;
    QRadioButton* rb_bottom_;

    // Hash / RNG
    QRadioButton* rb_djb2_;
    QRadioButton* rb_fnv1a_;
    QRadioButton* rb_lcg_;
    QRadioButton* rb_xorshift_;
    QLineEdit*    edit_capacity_;

    // Executar
    QPushButton*  btn_run_;

    // Resultados
    QGroupBox*    grp_results_;
    QTableWidget* table_;

    // Estatísticas
    QLabel* stat_total_;
    QLabel* stat_unique_;
    QLabel* stat_inserts_;
    QLabel* stat_collisions_;
    QLabel* stat_rehashes_;
    QLabel* stat_capacity_;
    QLabel* stat_load_;
    QLabel* stat_heap_ops_;
    QLabel* stat_time_;
    QLabel* stat_memory_;
};
