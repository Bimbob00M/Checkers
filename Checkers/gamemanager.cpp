#include "gamemanager.hpp"

GameManager::GameManager(QObject *_parent) : QObject(_parent)
{}

void GameManager::start()
{
    started = true;
    emit nextMove(type);
}

void GameManager::finish()
{
    started = false;
}

void GameManager::onEndOfMove()
{
    type = (type == type_t::White ? type_t::Black : type_t::White);
    emit nextMove(type);
}
