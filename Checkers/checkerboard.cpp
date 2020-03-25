#include "checkerboard.hpp"
#include <QPainter>
#include <QPaintEvent>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QDebug>
#include <QBitmap>

Checkerboard::Checkerboard(const int _boardEdgeSize, QWidget *parent)
    : QWidget(parent)
    , mainLayout(new QGridLayout(this))
    , oldSize(size())
    , boardEdgeSize(_boardEdgeSize)
{
    initBoard();
    setupLayout();
    arrangeCheckers(Checker::Type::White, Checker::Type::Black);

}

void Checkerboard::resizeEvent(QResizeEvent *_event)
{
    QWidget::resizeEvent(_event);
    checkAspectRatio();
}

void Checkerboard::initBoard()
{
    auto color = QColor(Qt::black);

    cells.reserve(boardEdgeSize);
    for (int row = 0; row < boardEdgeSize; ++row) {
        cells.push_back(boardEdge_t());
        cells[row].reserve(boardEdgeSize / 2);

        for (int col = 0; col < boardEdgeSize / 2; ++col) {
           auto cell = std::make_unique<Cell>(row, col, color, this);
           setConnections(cell.get());
           cells[row].push_back(std::move(cell));
        }
    }
}

void Checkerboard::setupLayout()
{
    mainLayout->setSpacing(0);
    mainLayout->setAlignment(Qt::AlignHCenter);

    if(!cells.empty()) {
        int rowNum = 0;
        for (const auto& row : cells) {
            int colNum = 0;
            int rowIndex = rowNum++;
            for (const auto& cell : row) {
                int colIndex = colNum++ * 2;
                if(rowIndex % 2 != 0) {
                    mainLayout->addWidget(cell.get(), rowIndex, colIndex);
                }
                else {
                    mainLayout->addWidget(cell.get(), rowIndex, colIndex + 1);
                }
            }
        }
    }
    setLayout(mainLayout);
}

void Checkerboard::checkAspectRatio()
{
    if((size().width() < size().height() && oldSize.width() > oldSize.height()) ||
        (size().width() > size().height() && oldSize.width() < oldSize.height()))
    {
        oldSize = size();
        changeLayoutAligment();
    }
}

void Checkerboard::arrangeCheckers(Type _bottomPlayer, Type _topPlayer)
{
    auto arrange = [&](int _fromRow, int _toRow, QPixmap& _pixmap, Type _type, Checker::MoveDirection _direction) {
        auto checkerImage = QSharedPointer<QPixmap>().create(_pixmap);
        checkerImage->setMask(checkerImage->createMaskFromColor(Qt::white));

        for(int row = _fromRow; row < _toRow; ++row) {
            for (int col = 0; col < boardEdgeSize / 2; ++col) {
                auto checker = std::make_unique<Checker>(row, col, checkerImage, _type, _direction);
                setConnections(checker.get());
                cells[row][col]->setChecker(std::move(checker));
            }
        }
    };

    //temp
    QPixmap bottomPlayerPixmap;
    QPixmap topPlayerPixmap;

    if(_bottomPlayer == Type::White) {
        bottomPlayerPixmap = QPixmap(":/qrc/resources/images/white checker.bmp");
        topPlayerPixmap = QPixmap(":/qrc/resources/images/black checker.bmp");
    }
    else {
        topPlayerPixmap = QPixmap(":/qrc/resources/images/white checker.bmp");
        bottomPlayerPixmap = QPixmap(":/qrc/resources/images/black checker.bmp");
    }
    //

    arrange(0, 3, topPlayerPixmap, _topPlayer, Checker::MoveDirection::Down);
    arrange(5, boardEdgeSize, bottomPlayerPixmap, _bottomPlayer, Checker::MoveDirection::Up);
}

void Checkerboard::setConnections(Cell *_cell)
{
    connect(_cell, &Cell::cellSelected, this, &Checkerboard::onCellSelected);
    connect(_cell, &Cell::cellDeselected, this, &Checkerboard::onCellUnselected);
    connect(_cell, &Cell::checkerMoved, this, &Checkerboard::onCheckerMoved);
    connect(_cell, &Cell::checkerJumped, this, &Checkerboard::onCheckerJumped);
}

void Checkerboard::setConnections(Checker *_checker)
{
    connect(_checker, &Checker::canMove, this, &Checkerboard::onCanMove);
    connect(_checker, &Checker::readyMove, this, &Checkerboard::onReadyMove);
    connect(_checker, &Checker::canJump, this, &Checkerboard::onCanJump);
    connect(_checker, &Checker::readyJump, this, &Checkerboard::onReadyJump);
    connect(this, &Checkerboard::checkJump, _checker, &Checker::onCheckJump);
    connect(this, &Checkerboard::checkMove, _checker, &Checker::onCheckMove);
}

void Checkerboard::resetOpenedCells()
{
    for (auto cell : openedCells) {
        cell->closeForMove();
        cell->closeForJump();
        cell->deactivate();
        cell->setSelected(false);
        cell->setBacklight(false);
        cell->update();
    }
    openedCells.clear();
}

void Checkerboard::resetActivatedCells()
{
    for (auto cell : activatedCells) {
        cell->deactivate();
        cell->setSelected(false);
        cell->setBacklight(false);
        cell->update();
    }
    activatedCells.clear();
}

void Checkerboard::resetCheckersForDestruction()
{
    for (const auto& i : checkersForDestruction) {
        auto cell = cells[i.desctinationIndex.first][i.desctinationIndex.second].get();
        cell->closeForJump();
    }
    checkersForDestruction.clear();
}

void Checkerboard::changeLayoutAligment()
{
    if(height() < width())
        mainLayout->setAlignment(Qt::AlignHCenter);
    else
        mainLayout->setAlignment(Qt::AlignVCenter);
}

void Checkerboard::onNextMove(Checker::Type _type)
{
    emit checkJump(_type, cells);

    if(activatedCells.isEmpty())
        emit checkMove(_type, cells);

    if(activatedCells.isEmpty())
        emit noMoves(_type);
}

void Checkerboard::onCanMove(const index_t &_index)
{
   auto cell = cells[_index.first][_index.second].get();   
   cell->activate();
   cell->setBacklight(true);
   cell->update();
   if(!activatedCells.contains(cell))
       activatedCells.append(cell);
}

void Checkerboard::onCanJump(const index_t &_index)
{
    onCanMove(_index);
}

void Checkerboard::onReadyMove(const QVector<index_t> &_indeces)
{
    for (const auto& i : _indeces) {
        auto cell = cells[i.first][i.second].get();
        cell->activate();
        cell->openForMove();
        openedCells.append(cell);
    }
}

void Checkerboard::onReadyJump(const QVector<Checker::JumpData> &_indeces)
{
    for(const auto& i : _indeces) {
        auto cell = cells[i.desctinationIndex.first][i.desctinationIndex.second].get();
        cell->activate();
        cell->openForJump();
        openedCells.append(cell);

        checkersForDestruction.append(i);
    }
}

void Checkerboard::onCellSelected(const index_t &_index)
{
    auto sender = cells[_index.first][_index.second].get();

    for (auto cell : activatedCells) {        
        if(cell != sender) {
            cell->setSelected(false);
            cell->setBacklight(false);
        }
        else {
            resetOpenedCells();

            sender->getChecker()->getJumpWays(cells);
            if(openedCells.isEmpty())
                sender->getChecker()->getMoveWays(cells);
        }
        cell->update();
    }
}

void Checkerboard::onCellUnselected()
{
    for (auto cell : activatedCells) {
        cell->setBacklight(true);
        cell->update();
    }
    resetOpenedCells();
    resetCheckersForDestruction();
}

void Checkerboard::onCheckerMoved(const index_t &_index)
{
    //qDebug() << "onCheckerMoved " << _index;
    auto sender = cells[_index.first][_index.second].get();

    for (auto cell : activatedCells) {
        if(cell->isSelected()) {
            cell->moveCheckerTo(sender);

            resetActivatedCells();
            resetOpenedCells();

            emit endOfMove();
            return;
        }
    }
}

void Checkerboard::onCheckerJumped(const index_t &_index)
{
    auto sender = cells[_index.first][_index.second].get();
    Cell* destr = nullptr;

    for (const auto& indices : checkersForDestruction) {
        if(_index == indices.desctinationIndex) {
            const auto& i = indices.destructionIndex;
            destr = cells[i.first][i.second].get();
        }
    }

    for (auto cell : activatedCells) {
        if(cell->isSelected()) {
            cell->moveCheckerTo(sender, destr);
            destr->update();
            destr = nullptr;

            resetActivatedCells();
            resetOpenedCells();
            resetCheckersForDestruction();

            const auto& type = sender->getChecker()->getType();
            sender->getChecker()->onCheckJump(type, cells);
            if(activatedCells.isEmpty())
                emit endOfMove();
            return;
        }
    }
}

int Checkerboard::getBoardSize() const
{
    return boardEdgeSize;
}
