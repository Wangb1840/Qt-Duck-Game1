#include "obstacle.h"
#include <QDebug>

Obstacle::Obstacle(QWidget *parent)
    : QWidget(parent)
    , currentX(0)
    , speed(8)
    , currentType(Type_Barrier)
    , animationFrame(0)
    , animTimer(new QTimer(this))
{
    setFixedSize(60, 70);
    setAttribute(Qt::WA_TranslucentBackground);
    connect(animTimer, &QTimer::timeout, this, [this]() {
        animationFrame = (animationFrame + 1) % 4;
        update();
    });
    animTimer->start(200);
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
    return QRect(currentX + 10, y() + 10, width() - 20, height() - 20);
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

void Obstacle::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    int groundY = (parentWidget() ? parentWidget()->height() : this->height()) - 100;

    switch (currentType) {
    case Type_Barrier: {
        painter.setBrush(QBrush(QColor(139, 90, 43)));
        painter.setPen(QPen(QColor(100, 60, 20), 3));
        painter.drawRect(10, 20, 40, 50);

        painter.setBrush(QBrush(QColor(160, 110, 50)));
        painter.setPen(QPen(QColor(100, 60, 20), 2));
        painter.drawRect(15, 25, 30, 15);

        painter.setBrush(QBrush(QColor(120, 80, 30)));
        painter.drawRect(20, 45, 8, 20);
        painter.drawRect(32, 45, 8, 20);

        painter.setBrush(QBrush(QColor(255, 50, 50)));
        painter.setPen(Qt::NoPen);
        painter.drawEllipse(18, 10, 12, 12);
        painter.drawEllipse(30, 10, 12, 12);
        break;
    }

    case Type_Flying: {
        painter.setBrush(QBrush(QColor(80, 80, 100)));
        painter.setPen(QPen(QColor(50, 50, 70), 2));
        painter.drawEllipse(10, 20, 40, 30);

        painter.setBrush(QBrush(QColor(60, 60, 80)));
        painter.drawEllipse(15, 25, 30, 20);

        QPoint beak1(45, 32);
        QPoint beak2(55, 35);
        QPoint beak3(45, 38);
        QPolygon beak;
        beak << beak1 << beak2 << beak3;
        painter.setBrush(QBrush(QColor(255, 180, 50)));
        painter.drawPolygon(beak);

        int wingOffset = (animationFrame % 2) * 5;
        painter.setBrush(QBrush(QColor(100, 100, 120)));
        painter.drawEllipse(5 - wingOffset, 15, 15, 20);
        painter.drawEllipse(40 + wingOffset, 15, 15, 20);
        break;
    }

    case Type_Ground: {
        painter.setBrush(QBrush(QColor(100, 70, 50)));
        painter.setPen(QPen(QColor(70, 40, 20), 2));

        QPolygon spikes;
        spikes << QPoint(5, 65)
               << QPoint(15, 35 + animationFrame * 2)
               << QPoint(25, 65)
               << QPoint(35, 30 - animationFrame * 2)
               << QPoint(45, 65)
               << QPoint(55, 35 + animationFrame * 2);
        painter.drawPolygon(spikes);

        painter.setBrush(QBrush(QColor(80, 50, 30)));
        painter.drawRect(10, 60, 40, 10);
        break;
    }
    }

    QWidget::paintEvent(event);
}
