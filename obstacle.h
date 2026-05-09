#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QWidget>
#include <QPainter>
#include <QRect>
#include <QTimer>
#include <QImage>

enum ObstacleType {
    Type_Barrier,
    Type_Flying,
    Type_Ground
};

class Obstacle : public QWidget
{
    Q_OBJECT

public:
    explicit Obstacle(QWidget *parent = nullptr);
    ~Obstacle();

    void setSpeed(int newSpeed);
    int getSpeed() const;

    void moveLeft();
    QRect getCollisionRect() const;

    void reset();
    void setObstacleType(ObstacleType type);

    void setPosition(int x, int y);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int currentX;
    int speed;
    ObstacleType currentType;
    int animationFrame;
    QTimer *animTimer;
    QImage obstacleImage;
};

#endif

