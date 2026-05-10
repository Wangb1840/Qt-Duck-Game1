#ifndef PLAYER_H
#define PLAYER_H

#include <QWidget>
#include <QPainter>
#include <QImage>
#include <QRect>

class Player : public QWidget
{
    Q_OBJECT

public:
    explicit Player(QWidget *parent = nullptr);
    ~Player();

    void jump();
    void startRunning();
    void stopRunning();
    void setDashing(bool dash);
    bool isDashing() const;

    QRect getCollisionRect() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QImage playerImage;
    QImage dashImage;
    bool isJumping;
    bool isRunning;
    bool isDashingState;
    double jumpVelocity;
    int groundY;
    int currentY;
    int frameIndex;
    int frameTimer;
    static constexpr double JUMP_FORCE = 7.0;
    static constexpr double GRAVITY = 0.06;
    static const int PLAYER_WIDTH = 80;
    static const int PLAYER_HEIGHT = 80;
};

#endif
