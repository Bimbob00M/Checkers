#pragma once

#include "checker.hpp"

#include <QObject>

class GameManager : public QObject
{
    Q_OBJECT
    using type_t = Checker::Type;
public:
    GameManager(QObject *_parent = nullptr);

    void start();
    void finish();

signals:
    void nextMove(type_t _type);

public slots:
    void onEndOfMove();

private:

    bool started = false;
    type_t type = type_t::White;
};

