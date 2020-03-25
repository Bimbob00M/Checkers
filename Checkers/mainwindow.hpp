#pragma once

#include "gamemanager.hpp"
#include "checkerboard.hpp"
#include <QMainWindow>

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);

private:
    void setupUi();

private:
    Checkerboard *board;
    GameManager *manager;
};
