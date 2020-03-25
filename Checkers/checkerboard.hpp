#pragma once

#include "cell.hpp"
#include "checker.hpp"

#include <QWidget>
#include <QVector>
#include <QList>
#include <QGridLayout>

class Checkerboard : public QWidget
{
    Q_OBJECT
    using boardEdge_t = Checker::boardEdge_t;
    using Type = Checker::Type;
    using board_t = Checker::board_t;
    using index_t = Checker::index_t;

public:
    explicit Checkerboard(const int _boardEdgeSize = 8, QWidget *_parent = nullptr);

    int getBoardSize() const;
    void arrangeCheckers(Type _firstPlayer, Type _secondPlayer);

public slots:
    void onNextMove(Type _type);

signals:
    void noMoves(const Type &_type);
    void endOfMove();
    void aspectRatioChanged();
    void checkJump(const Type &_type, const board_t &_cells);
    void checkMove(const Type &_type, const board_t &_cells);

protected:
    void resizeEvent(QResizeEvent *_event) override;

private slots:
    void changeLayoutAligment();

    void onCanMove(const index_t &_index);
    void onCanJump(const index_t &_index);
    void onReadyMove(const QVector<index_t> &_indeces);
    void onReadyJump(const QVector<Checker::JumpData> &_indeces);

    void onCheckerMoved(const index_t &_index);
    void onCheckerJumped(const index_t &_index);

    void onCellSelected(const index_t &_index);
    void onCellUnselected();

private:
    void initBoard();
    void setupLayout();
    void checkAspectRatio();
    void setConnections(Cell *_cell);
    void setConnections(Checker *_checker);
    void resetOpenedCells();
    void resetActivatedCells();
    void resetCheckersForDestruction();

private:
    QGridLayout *mainLayout;
    board_t cells;
    QList<Cell*> activatedCells;
    QList<Cell*> openedCells;
    QList<Checker::JumpData> checkersForDestruction;
    QSize oldSize;
    const int boardEdgeSize;
};

