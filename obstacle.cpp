#include "obstacle.h"

Obstacle::Obstacle(QWidget *parent)
    : QWidget(parent)
    , currentX(0)
    , speed(8)
    , currentType(Type_Barrier)
{
    setFixedSize(80, 80);
    setAttribute(Qt::WA_TranslucentBackground);
}

Obstacle::~Obstacle()
{
}

void Obstacle::setSpeed(int newSpeed)
{
    speed = newSpeed;
}

int Obstacle::getSpeed() const
{
    return speed;
}

void Obstacle::moveLeft()
{
    currentX -= speed;
    move(currentX, y());
}

QRect Obstacle::getCollisionRect() const
{
    return QRect(currentX + 20, y() + 20, width() - 40, height() - 40);
}

void Obstacle::reset()
{
    currentX = (parentWidget() ? parentWidget()->width() : this->width()) + 50;
    move(currentX, y());
}

void Obstacle::setObstacleType(ObstacleType type)
{
    currentType = type;
    update();
}

void Obstacle::setPosition(int x, int y)
{
    currentX = x;
    move(x, y);
}

void Obstacle::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
}
