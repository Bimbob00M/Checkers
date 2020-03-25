#include "mainwindow.hpp"

#include <QApplication>
#include <QScreen>
#include <QPainter>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , board(new Checkerboard(8, this))
    , manager(new GameManager(this))
{
    connect(manager, &GameManager::nextMove, board, &Checkerboard::onNextMove);
    connect(board, &Checkerboard::endOfMove, manager, &GameManager::onEndOfMove);
    setupUi();
    manager->start();
}

void MainWindow::setupUi()
{
    const auto screenCenter = QApplication::screens().first()->availableGeometry().center();
    QRect r = frameGeometry();
    r.moveCenter(screenCenter);
    move(r.topLeft());

    setCentralWidget(board);
}
