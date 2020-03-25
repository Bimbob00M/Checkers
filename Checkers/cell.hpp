#pragma once

#include "checker.hpp"

#include <QWidget>
#include <QAction>

class Cell : public QWidget
{
    Q_OBJECT
    using index_t = Checker::index_t;
public:
    explicit Cell(const int _row, const int _col, const QColor &_color = QColor(Qt::black), QWidget *_parent = nullptr);
    explicit Cell(const index_t &_index, const QColor &_color = QColor(Qt::black), QWidget *_parent = nullptr);

    Cell(const Cell&) = delete;
    Cell& operator= (const Cell&) = delete;

    void draw(QPainter *_painter);

    void activate();
    void deactivate();
    void openForMove();
    void closeForMove();
    void openForJump();
    void closeForJump();

    const std::unique_ptr<Checker> &getChecker() const;
    void setChecker(std::unique_ptr<Checker> &&_value);
    void moveCheckerTo(Cell *_destinationCell);
    void moveCheckerTo(Cell *_destinationCell, Cell *_destructCell);

    bool hasChecker() const;
    bool isSelected() const;
    bool isOpenForMove() const;
    bool isOpenForJump() const;
    void setSelected(bool _value);
    void setBacklight(bool _value);

    const QPair<int, int>& getIndex() const;

signals:
    void cellClicked(Cell *_cell);
    void cellSelected(const index_t &_index);
    void cellDeselected();
    void checkerMoved(const index_t &_index);
    void checkerJumped(const index_t &_index);

protected:
    QSize sizeHint() const override;
    void paintEvent(QPaintEvent *_event) override;
    void resizeEvent(QResizeEvent *_event) override;
    void mousePressEvent(QMouseEvent *_event) override;
    void keyPressEvent(QKeyEvent *_event) override;


private:
    std::unique_ptr<Checker> checker;
    const index_t index;
    const QColor color;
    QSize prefferedSize;
    bool openedForMove;
    bool openedForJump;
    bool clickable;
    bool selected;
    bool backlight;
};

