#include "checker.hpp"
#include "cell.hpp"

Checker::Checker(const int row, const int col,
                 const QSharedPointer<QPixmap> &_image,
                 const Type _type, const MoveDirection _moveDir,
                 QObject *_parent)
    : QObject(_parent)
    , image(_image)
    , index(qMakePair(row, col))
    , type(_type)
    , moveDir(_moveDir)
{}

Checker::Checker(const index_t &_index,
                 const QSharedPointer<QPixmap> &_image,
                 const Type _type, const MoveDirection _moveDir,
                 QObject *_parent)
    : Checker(_index.first, _index.second, _image, _type, _moveDir, _parent)
{}

Checker::Checker(Checker &&_c)
    : Checker(_c.index, std::move(_c.image), _c.type, _c.moveDir, _c.parent())
{}

Checker &Checker::operator=(Checker &&_c)
{
    if(&_c == this)
        return *this;

    image.clear();
    image = std::move(_c.image);
    index = std::move(_c.index);
    type = _c.type;
    moveDir = _c.moveDir;

    return *this;
}

const QSharedPointer<QPixmap> &Checker::getImage() const
{
    return image;
}

Checker::Type Checker::getType() const
{
    return type;
}

void Checker::setType(const Type &_type)
{
    type = _type;
}

Checker::MoveDirection Checker::getMoveDir() const
{
    return moveDir;
}

void Checker::setMoveDir(const MoveDirection &_dir)
{
    moveDir = _dir;
}

const Checker::index_t &Checker::getIndex() const
{
    return index;
}

void Checker::setIndex(const QPair<int, int> &_index)
{
    index = _index;
}


void Checker::onCheckJump(const Type &_type, const board_t &_cells)
{
    if(_type != type)
        return;

    jumpCandidates.clear();

    const auto rowNum = _cells.size();
    const auto colNum = _cells[0].size();

    calculateJumps(rowNum, colNum, _cells, &Checker::getTopLeft);
    calculateJumps(rowNum, colNum, _cells, &Checker::getTopRight);
    calculateJumps(rowNum, colNum, _cells, &Checker::getBottomLeft);
    calculateJumps(rowNum, colNum, _cells, &Checker::getBottomRight);

    if(!jumpCandidates.isEmpty())
         emit canJump(index);
}

void Checker::getJumpWays(board_t &_cells)
{
    if(!jumpCandidates.isEmpty())
        emit readyJump(jumpCandidates);
}

void Checker::onCheckMove(const Checker::Type &_type, const board_t &_cells)
{
    if(_type != type)
        return;

    moveCandidates.clear();

    const auto rowNum = _cells.size();
    const auto colNum = _cells[0].size();

    if(moveDir == MoveDirection::Down) {
        calculateMoves(rowNum, colNum, _cells, &Checker::getBottomLeft);
        calculateMoves(rowNum, colNum, _cells, &Checker::getBottomRight);
    }
    else if(moveDir == MoveDirection::Up) {
        calculateMoves(rowNum, colNum, _cells, &Checker::getTopLeft);
        calculateMoves(rowNum, colNum, _cells, &Checker::getTopRight);
    }
    else {
        calculateMoves(rowNum, colNum, _cells, &Checker::getTopLeft);
        calculateMoves(rowNum, colNum, _cells, &Checker::getTopRight);
        calculateMoves(rowNum, colNum, _cells, &Checker::getBottomLeft);
        calculateMoves(rowNum, colNum, _cells, &Checker::getBottomRight);
    }

    if(!moveCandidates.isEmpty())
        emit canMove(index);
}

void Checker::getMoveWays(board_t &_cells)
{
    if(!moveCandidates.isEmpty())
        emit readyMove(moveCandidates);
}

bool Checker::isValidIndex(const index_t &_index, int _rowNum, int _colNum) const
{
    auto isValidRowIndex = (_index.first >= 0 && _index.first < _rowNum) ? true : false;
    auto isValidColIndex = (_index.second >= 0 && _index.second < _colNum) ? true : false;

    return isValidRowIndex && isValidColIndex;
}

void Checker::calculateJumps(const int _rowNum, const int _colNum, const board_t& _cells, std::function<index_t (Checker &, const index_t&)> _dirGetter)
{
    auto adjacentIndex = _dirGetter(*this, index);
    if(isValidIndex(adjacentIndex, _rowNum, _colNum)) {
        auto adjacentCell = _cells[adjacentIndex.first][adjacentIndex.second].get();
        if(adjacentCell->hasChecker()) {
            if(adjacentCell->getChecker()->getType() != type) {
                auto destinationIndex = _dirGetter(*adjacentCell->getChecker().get(), adjacentCell->getChecker()->getIndex());
                if(isValidIndex(destinationIndex, _rowNum, _colNum)) {
                    if(!_cells[destinationIndex.first][destinationIndex.second]->hasChecker())  {
                        auto data = JumpData(adjacentIndex, destinationIndex);
                        jumpCandidates.push_back(data);
                    }
                }
            }
        }
    }
}

void Checker::calculateMoves(const int _rowNum, const int _colNum, const board_t &_cells, std::function<index_t(Checker &, const index_t&)> _dirGetter)
{
    auto adjacentIndex = _dirGetter(*this, index);
    if(isValidIndex(adjacentIndex, _rowNum, _colNum)) {
        if(!_cells[adjacentIndex.first][adjacentIndex.second]->hasChecker())
            moveCandidates.push_back(adjacentIndex);
    }
}

Checker::index_t Checker::getTopLeft(const index_t &_baseIndex) const
{
    auto second = (_baseIndex.first % 2) != 0 ? _baseIndex.second - 1 : _baseIndex.second;
    return qMakePair(_baseIndex.first - 1, second);
}

Checker::index_t Checker::getTopRight(const index_t &_baseIndex) const
{
    auto second = (_baseIndex.first % 2) != 0 ? _baseIndex.second : _baseIndex.second + 1;
    return qMakePair(_baseIndex.first - 1, second);
}

Checker::index_t Checker::getBottomLeft(const index_t &_baseIndex) const
{
    auto second = (_baseIndex.first % 2) != 0 ? _baseIndex.second - 1 : _baseIndex.second;
    return qMakePair(_baseIndex.first + 1, second);
}

Checker::index_t Checker::getBottomRight(const index_t &_baseIndex) const
{
    auto second = (_baseIndex.first % 2) != 0 ? _baseIndex.second : _baseIndex.second + 1;
    return qMakePair(_baseIndex.first + 1, second);
}

Checker::index_t Checker::topLeft() const
{
    return getTopLeft(index);
}

Checker::index_t Checker::topRight() const
{
    return getTopRight(index);
}

Checker::index_t Checker::bottomLeft() const
{
    return getBottomLeft(index);
}

Checker::index_t Checker::bottomRight() const
{
    return getBottomRight(index);
}




