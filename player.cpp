#include "player.h"
#include <QDebug>

Player::Player(QWidget *parent)
    : QWidget(parent)
    , isJumping(false)
    , isRunning(false)
    , isDashingState(false)
    , jumpHeight(0)
    , groundY(0)
    , currentY(0)
    , frameIndex(0)
    , frameTimer(0)
{
    setFixedSize(PLAYER_WIDTH, PLAYER_HEIGHT);
    setAttribute(Qt::WA_TranslucentBackground);
}

Player::~Player()
{
}

void Player::jump()
{
    if (!isJumping && isRunning) {
        isJumping = true;
        jumpHeight = JUMP_SPEED;
    }
}

void Player::startRunning()
{
    isRunning = true;
}

void Player::stopRunning()
{
    isRunning = false;
}

void Player::setDashing(bool dash)
{
    isDashingState = dash;
}

bool Player::isDashing() const
{
    return isDashingState;
}

QRect Player::getCollisionRect() const
{
    int y = currentY;
    if (!isJumping) {
        y = groundY - PLAYER_HEIGHT;
    }
    return QRect(10, y + 15, PLAYER_WIDTH - 20, PLAYER_HEIGHT - 25);
}

void Player::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    groundY = parentWidget()->height() - 100;
    if (!isJumping) {
        currentY = groundY - PLAYER_HEIGHT;
    }

    if (isJumping) {
        currentY -= jumpHeight;
        jumpHeight -= GRAVITY;
        if (currentY >= groundY - PLAYER_HEIGHT) {
            currentY = groundY - PLAYER_HEIGHT;
            isJumping = false;
            jumpHeight = 0;
        }
    }

    int drawY = currentY;
    if (isJumping) {
        painter.save();
        painter.translate(0, drawY);
    }

    if (isDashingState) {
        painter.setBrush(QBrush(QColor(255, 100, 100)));
        painter.setPen(QPen(QColor(200, 50, 50), 3));
    } else {
        painter.setBrush(QBrush(QColor(255, 200, 100)));
        painter.setPen(QPen(QColor(200, 150, 50), 3));
    }
    painter.drawEllipse(15, 20, 50, 45);

    painter.setBrush(QBrush(QColor(255, 220, 120)));
    painter.setPen(Qt::NoPen);
    painter.drawEllipse(20, 25, 40, 35);

    if (isDashingState) {
        painter.setBrush(QBrush(QColor(255, 50, 50)));
    } else {
        painter.setBrush(QBrush(QColor(100, 100, 100)));
    }
    painter.drawEllipse(25, 30, 12, 12);
    painter.drawEllipse(43, 30, 12, 12);

    QPoint eyeWhite1(28, 33);
    QPoint eyeWhite2(46, 33);
    painter.setBrush(Qt::white);
    painter.drawEllipse(eyeWhite1, 5, 5);
    painter.drawEllipse(eyeWhite2, 5, 5);

    if (isDashingState) {
        painter.setBrush(Qt::red);
    } else {
        painter.setBrush(Qt::black);
    }
    painter.drawEllipse(29, 34, 3, 3);
    painter.drawEllipse(47, 34, 3, 3);

    QPoint beak1(38, 40);
    QPoint beak2(48, 40);
    QPoint beak3(43, 47);
    QPolygon beak;
    beak << beak1 << beak2 << beak3;
    painter.setBrush(QBrush(QColor(255, 150, 50)));
    painter.setPen(QPen(QColor(200, 100, 30), 1));
    painter.drawPolygon(beak);

    painter.setBrush(QBrush(QColor(255, 200, 100)));
    painter.setPen(QPen(QColor(200, 150, 50), 2));

    if (isDashingState) {
        frameTimer++;
        if (frameTimer >= 3) {
            frameTimer = 0;
            frameIndex = (frameIndex + 1) % 2;
        }
        int legOffset = frameIndex == 0 ? 0 : 5;
        painter.drawLine(30, 60, 25 - legOffset, 75);
        painter.drawLine(50, 60, 55 + legOffset, 75);
    } else {
        painter.drawLine(30, 60, 25, 75);
        painter.drawLine(50, 60, 55, 75);
    }

    if (isDashingState) {
        painter.setBrush(QBrush(QColor(100, 200, 255, 150)));
        for (int i = 0; i < 3; i++) {
            painter.drawEllipse(70 + i * 15, 35 + i * 5, 20 - i * 5, 15 - i * 3);
        }
    }

    if (isDashingState) {
        painter.restore();
    }

    if (isJumping) {
        painter.setPen(QPen(QColor(150, 150, 150, 100), 2, Qt::DashLine));
        painter.drawLine(40, groundY, 40, groundY + 10);
        painter.drawLine(40, groundY + 10, 20, groundY + 10);
        painter.drawLine(40, groundY + 10, 60, groundY + 10);
    }
}
