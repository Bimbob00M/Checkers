#include "king.hpp"
#include "cell.hpp"

King::King(const int _row, const int _col, const QSharedPointer<QPixmap> &_image,
           const Type _type, const MoveDirection _moveDir, QObject *_parent)
    : Checker(_row, _col, _image, _type, _moveDir, _parent)
{}

King::King(const index_t &_index, const QSharedPointer<QPixmap> &_image,
           const Type _type, const MoveDirection _moveDir, QObject *_parent)
    : Checker(_index, _image, _type, _moveDir, _parent)
{}

void King::calculateJumps(const int _rowNum, const int _colNum, const board_t &_cells, std::function<index_t (Checker &, const index_t&)> _dirGetter)
{

}

void King::calculateMoves(const int _rowNum, const int _colNum, const board_t &_cells, std::function<index_t (Checker &, const index_t&)> _dirGetter)
{

}

//const QVector<Checker::index_t>& King::recursivelyСalculateMoves(const int _rowNum, const int _colNum, const board_t &_cells,
//                                                                 std::function<index_t (Checker &, const index_t&)> _dirGetter)
//{
//    QVector<index_t> way;
//    auto index = _dirGetter(*this, index);
//
//}
