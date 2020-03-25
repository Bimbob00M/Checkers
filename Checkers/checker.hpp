#pragma once

#include <QObject>
#include <QSharedPointer>
#include <QImage>

class Cell;

class Checker : public QObject
{
    Q_OBJECT

public:
    using boardEdge_t = std::vector<std::unique_ptr<Cell>>;
    using board_t = std::vector<boardEdge_t>;
    using index_t = QPair<int, int>;

    enum class Type { White, Black };
    enum class MoveDirection { Up, Down, Both };

    struct JumpData {
        JumpData(index_t _destruct, index_t _desctin)
            : destructionIndex(_destruct)
            , desctinationIndex(_desctin)
        {}
        index_t destructionIndex;
        index_t desctinationIndex;
    };

    explicit Checker(const int row, const int col, const QSharedPointer<QPixmap> &_image, const Type _type, const MoveDirection _moveDir, QObject *_parent = nullptr);
    explicit Checker(const index_t &_index, const QSharedPointer<QPixmap> &_image, const Type _type, const MoveDirection _moveDir, QObject *_parent = nullptr);

    virtual ~Checker() = default;

    Checker(Checker&& _c);
    Checker& operator=(Checker&& _c);

    Checker(const Checker& _c) = delete;
    Checker& operator=(const Checker& _c) = delete;

    const QSharedPointer<QPixmap>& getImage() const;

    Type getType() const;
    void setType(const Type &_type);

    MoveDirection getMoveDir() const;
    void setMoveDir(const MoveDirection &_dir);

    const index_t& getIndex() const;
    void setIndex(const index_t &_index);

    index_t topLeft() const;
    index_t topRight() const;
    index_t bottomLeft() const;
    index_t bottomRight() const;

signals:
    void canMove(const index_t &_index);
    void canJump(const index_t &_index);
    void readyMove(const QVector<index_t> &_indeces);
    void readyJump(const QVector<JumpData> &_indeces);

public slots:
    void onCheckJump(const Type &_type, const board_t &_cells);
    void getJumpWays(board_t &_cells);

    void onCheckMove(const Type &_type, const board_t &_cells);
    void getMoveWays(board_t &_cells);

protected:
    bool isValidIndex(const index_t &_index, int _rowNum, int _colNum) const;

    virtual void calculateJumps(const int _rowNum, const int _colNum, const board_t &_cells, std::function<index_t(Checker &, const index_t&)> _dirGetter);
    virtual void calculateMoves(const int _rowNum, const int _colNum, const board_t &_cells, std::function<index_t(Checker &, const index_t&)> _dirGetter);

    index_t getTopLeft(const index_t& _baseIndex) const;
    index_t getTopRight(const index_t& _baseIndex) const;
    index_t getBottomLeft(const index_t& _baseIndex) const;
    index_t getBottomRight(const index_t& _baseIndex) const;

protected:
    QVector<JumpData> jumpCandidates;
    QVector<index_t> moveCandidates;
    QSharedPointer<QPixmap> image;
    index_t index;
    Type type;
    MoveDirection moveDir;
};




