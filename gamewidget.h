#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include <QWidget>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QFont>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include "player.h"
#include "obstacle.h"

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = nullptr);
    ~GameWidget();

protected:
    void paintEvent(QPaintEvent *event) override;
    bool event(QEvent *event) override;

private slots:
    void onGameUpdate();
    void onStartGame();
    void onRestartGame();
    void onDashCooldown();

private:
    void initGame();
    void startGameLoop();
    void stopGameLoop();
    void generateObstacle();
    void checkCollisions();
    void updateScore();
    void gameOver();
    void drawBackground(QPainter &painter);
    void drawGround(QPainter &painter);

    Player *player;
    QList<Obstacle*> obstacles;
    QTimer *gameTimer;
    QTimer *scoreTimer;
    QTimer *dashCooldownTimer;

    int score;
    int highScore;
    int gameSpeed;
    int dashCooldown;
    bool isGameRunning;
    bool isGameOver;
    bool canDash;

    QLabel *scoreLabel;
    QLabel *highScoreLabel;
    QLabel *dashLabel;
    QLabel *gameOverLabel;
    QLabel *tipLabel;
    QPushButton *restartButton;
    QPushButton *startButton;

    int groundOffset;
    int bgOffset1;
    int bgOffset2;

    static const int GAME_WIDTH = 900;
    static const int GAME_HEIGHT = 500;
    static const int GROUND_HEIGHT = 100;
    static const int DASH_COOLDOWN_TIME = 3000;
};

#endif
