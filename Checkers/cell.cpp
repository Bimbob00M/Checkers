#include "cell.hpp"
#include "checkerboard.hpp"

#include <QPainter>
#include <QDebug>
#include <QKeyEvent>

Cell::Cell(const int _row, const int _col, const QColor &_color, QWidget *_parent)
    : QWidget(_parent)
    , index(qMakePair(_row, _col))
    , color(_color)
    , prefferedSize(size())
    , openedForMove(false)
    , openedForJump(false)
    , clickable(false)
    , selected(false)
    , backlight(false)
{
    setMinimumSize(64, 64);
    QSizePolicy policy(this->sizePolicy());
    policy.setVerticalPolicy(QSizePolicy::Preferred);
    policy.setHorizontalPolicy(QSizePolicy::Preferred);
    setSizePolicy(policy);
    setFocusPolicy(Qt::ClickFocus);
}

Cell::Cell(const index_t &_index, const QColor &_color, QWidget *_parent)
    : Cell(_index.first, _index.second, _color, _parent)
{}

QSize Cell::sizeHint() const
{
    auto size = qMin(prefferedSize.height(), prefferedSize.width());
    return QSize(size, size);
}

void Cell::paintEvent(QPaintEvent *_event)
{
    QWidget::paintEvent(_event);

    QPainter p(this);
    p.setRenderHint(QPainter::RenderHint::HighQualityAntialiasing);
    draw(&p);
}

void Cell::draw(QPainter *_painter)
{
    _painter->save();
    _painter->setPen(QPen(Qt::transparent));

    QRadialGradient grad(rect().center(), rect().width() / 2);
    auto setupGradient = [&](const QColor &_mainColor) {
        grad.setColorAt(0.15, _mainColor);
        grad.setColorAt(0.3, color);
        grad.setColorAt(0.55, _mainColor);
        grad.setColorAt(0.7, color);
        grad.setColorAt(0.9, _mainColor);
        grad.setColorAt(1, color);
    };

    if(openedForJump) {
        setupGradient(Qt::red);
        _painter->setBrush(grad);
    }
    else if(openedForMove) {
        setupGradient(Qt::green);
        _painter->setBrush(grad);
    }
    else if(backlight) {
        grad.setColorAt(0.9, Qt::green);
        grad.setColorAt(1, color);
        _painter->setBrush(grad);
    }
    else {
        _painter->setBrush(color);
    }
    _painter->drawRect(rect());

    if(checker) {
        auto image = checker->getImage()->scaled(size());
        _painter->drawPixmap(rect(), image);
    }
    _painter->restore();
}

void Cell::resizeEvent(QResizeEvent *_event)
{
    QWidget::resizeEvent(_event);

    //temp
    auto par = qobject_cast<Checkerboard*>(parentWidget());
    if(par)
        prefferedSize = par->size() / par->getBoardSize();
    else
        prefferedSize = QSize(64, 64);
    updateGeometry();
    //
}

void Cell::mousePressEvent(QMouseEvent *_event)
{
    QWidget::mousePressEvent(_event);

    if(clickable) {
        selected = !selected;
        if(selected) {
            if(openedForJump) {
                emit checkerJumped(index);
                return;
            }
            if (openedForMove) {
                emit checkerMoved(index);
                return;
            }
            backlight = true;
            emit cellSelected(index);
            return;
        }
        backlight = false;
        emit cellDeselected();
    }
}

void Cell::keyPressEvent(QKeyEvent *_event)
{    
    QWidget::keyPressEvent(_event);

    if(selected) {
        if(_event->key() == Qt::Key_Escape) {
            selected = false;
            backlight = false;
            emit cellDeselected();
        }
    }
}

void Cell::moveCheckerTo(Cell *_destinationCell)
{
    checker->setIndex(_destinationCell->getIndex());
    _destinationCell->setChecker(std::move(checker));
}

void Cell::moveCheckerTo(Cell *_destinationCell, Cell *_destructCell)
{
    moveCheckerTo(_destinationCell);
    _destructCell->checker.reset();
}

void Cell::activate()
{
   clickable = true;
}

void Cell::deactivate()
{
    clickable = false;
}

void Cell::openForMove()
{
    openedForMove = true;
    update();
}

void Cell::closeForMove()
{
    openedForMove = false;
    update();
}

void Cell::openForJump()
{
    openedForJump = true;
    update();
}

void Cell::closeForJump()
{
    openedForJump = false;
    update();
}

bool Cell::isOpenForMove() const
{
    return openedForMove;
}

bool Cell::isOpenForJump() const
{
    return  openedForJump;
}

const Cell::index_t& Cell::getIndex() const
{
    return index;
}

void Cell::setBacklight(bool _value)
{
    backlight = _value;
}

bool Cell::isSelected() const
{
    return selected;
}

void Cell::setSelected(bool _value)
{
    selected = _value;
}

void Cell::setChecker(std::unique_ptr<Checker> &&_value)
{
    checker = std::move(_value);
}

bool Cell::hasChecker() const
{
    return checker.get();
}

const std::unique_ptr<Checker>& Cell::getChecker() const
{
    return checker;
}


