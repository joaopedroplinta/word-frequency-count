#include <QApplication>
#include "main_window.hpp"

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("WordCount");
    app.setApplicationDisplayName("WordCount — Frequência de Palavras");
    MainWindow w;
    w.show();
    return app.exec();
}
