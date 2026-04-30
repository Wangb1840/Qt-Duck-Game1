#ifndef OBSTACLE_H
#define OBSTACLE_H

#include <QWidget>
#include <QPainter>
#include <QRect>
#include <QTimer>

class Obstacle : public QWidget
{
    Q_OBJECT

public:
    explicit Obstacle(QWidget *parent = nullptr);
    ~Obstacle();

    void setSpeed(int speed);
    int getSpeed() const;
    void moveLeft();
    QRect getCollisionRect() const;
    void reset();

    enum ObstacleType {
        Type_Barrier,
        Type_Flying,
        Type_Ground
    };
    void setObstacleType(ObstacleType type);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int currentX;
    int speed;
    ObstacleType currentType;
    int animationFrame;
    QTimer *animTimer;
};

#endif
