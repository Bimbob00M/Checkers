#pragma once

#include "checker.hpp"

class King : public Checker
{
public:
    explicit King(const int _row, const int _col, const QSharedPointer<QPixmap> &_image,
                  const Type _type, const MoveDirection _moveDir, QObject *_parent);

    explicit King(const index_t &_index, const QSharedPointer<QPixmap> &_image,
                  const Type _type, const MoveDirection _moveDir, QObject *_parent);


    void calculateJumps(const int _rowNum, const int _colNum, const board_t &_cells, std::function<index_t (Checker &, const index_t&)> _dirGetter) override;
    void calculateMoves(const int _rowNum, const int _colNum, const board_t &_cells, std::function<index_t (Checker &, const index_t&)> _dirGetter) override;

private:
//    const QVector<index_t>& recursivelyСalculateMoves(const int _rowNum, const int _colNum,
//                                                      const board_t &_cells, std::function<index_t (Checker &, const index_t&)> _dirGetter);
};

