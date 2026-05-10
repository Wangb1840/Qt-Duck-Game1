#include "player.h"
#include <QDebug>
#include <QImage>

Player::Player(QWidget *parent)
    : QWidget(parent)
    , playerImage()
    , dashImage()
    , isJumping(false)
    , isRunning(false)
    , isDashingState(false)
    , jumpVelocity(0.0)
    , groundY(0)
    , currentY(0)
    , frameIndex(0)
    , frameTimer(0)
{
    setFixedSize(PLAYER_WIDTH, PLAYER_HEIGHT);
    setAttribute(Qt::WA_TranslucentBackground);

    bool loaded = playerImage.load(":/images/jiangbanya.jpg");
    if (loaded && !playerImage.isNull()) {
        playerImage = playerImage.scaled(PLAYER_WIDTH, PLAYER_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    } else {
        playerImage = QImage(PLAYER_WIDTH, PLAYER_HEIGHT, QImage::Format_ARGB32);
        playerImage.fill(Qt::transparent);
        QPainter p(&playerImage);
        p.setBrush(QBrush(QColor(255, 100, 100)));
        p.setPen(QPen(QColor(200, 50, 50), 2));
        p.drawEllipse(10, 15, 60, 50);

        p.setBrush(QBrush(QColor(255, 150, 100)));
        p.drawEllipse(25, 10, 15, 20);
        p.drawEllipse(45, 10, 15, 20);

        p.setBrush(Qt::white);
        p.drawEllipse(28, 30, 10, 10);
        p.drawEllipse(44, 30, 10, 10);
        p.setBrush(Qt::black);
        p.drawEllipse(31, 33, 4, 4);
        p.drawEllipse(47, 33, 4, 4);

        p.setBrush(QBrush(QColor(255, 180, 50)));
        p.drawEllipse(38, 40, 8, 6);
    }
}

Player::~Player()
{
}

void Player::jump()
{
    if (!isJumping && isRunning) {
        isJumping = true;
        jumpVelocity = JUMP_FORCE;
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
    return QRect(x() + 5, y + 10, PLAYER_WIDTH - 10, PLAYER_HEIGHT - 15);
}

void Player::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (groundY == 0) {
        groundY = (parentWidget() ? parentWidget()->height() : this->height()) - 100;
        currentY = groundY - PLAYER_HEIGHT;
        move(x(), currentY);
    }

    if (isJumping) {
        currentY -= (int)jumpVelocity;
        jumpVelocity -= GRAVITY;
        if (currentY >= groundY - PLAYER_HEIGHT) {
            currentY = groundY - PLAYER_HEIGHT;
            isJumping = false;
            jumpVelocity = 0.0;
        }
        move(x(), currentY);
    }

    painter.drawImage(0, 0, playerImage);

    if (isDashingState) {
        painter.setBrush(QBrush(QColor(255, 100, 100, 150)));
        for (int i = 0; i < 3; i++) {
            painter.drawEllipse(70 + i * 15, 35 + i * 5, 20 - i * 5, 15 - i * 3);
        }
    }
}
