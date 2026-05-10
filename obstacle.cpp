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
    static QImage obstacleImage;
    static bool loaded = false;
    if (!loaded) {
        obstacleImage.load(":/images/baihu.jpg");
        if (!obstacleImage.isNull()) {
            obstacleImage = obstacleImage.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        }
        loaded = true;
    }

    int imgWidth = obstacleImage.width();
    int imgHeight = obstacleImage.height();
    int offsetX = (width() - imgWidth) / 2;
    int offsetY = height() - imgHeight;

    return QRect(currentX + offsetX, y() + offsetY, imgWidth, imgHeight);
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
