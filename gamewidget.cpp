#include "gamewidget.h"
#include <QPainter>
#include <QPainterPath>
#include <QLinearGradient>
#include <QDebug>
#include <QRandomGenerator>

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent)
    , player(nullptr)
    , gameTimer(new QTimer(this))
    , scoreTimer(new QTimer(this))
    , dashCooldownTimer(new QTimer(this))
    , score(0)
    , highScore(0)
    , gameSpeed(8)
    , dashCooldown(0)
    , isGameRunning(false)
    , isGameOver(false)
    , canDash(true)
    , groundOffset(0)
    , bgOffset1(0)
    , bgOffset2(0)
{
    setFixedSize(GAME_WIDTH, GAME_HEIGHT);
    setWindowTitle("酱板鸭快跑 - 逃跑吧！美味的鸭子！");
    setStyleSheet("QWidget { background-color: #87CEEB; }");

    QFont titleFont("Microsoft YaHei", 24, QFont::Bold);
    QFont normalFont("Microsoft YaHei", 14);
    QFont scoreFont("Microsoft YaHei", 18, QFont::Bold);
    QFont tipFont("Microsoft YaHei", 12);

    scoreLabel = new QLabel(this);
    scoreLabel->setFont(scoreFont);
    scoreLabel->setStyleSheet("QLabel { color: #FFFFFF; background-color: rgba(0,0,0,100); border-radius: 10px; padding: 5px 15px; }");
    scoreLabel->setText("分数: 0");
    scoreLabel->setGeometry(20, 20, 150, 40);

    highScoreLabel = new QLabel(this);
    highScoreLabel->setFont(normalFont);
    highScoreLabel->setStyleSheet("QLabel { color: #FFD700; background-color: rgba(0,0,0,100); border-radius: 8px; padding: 3px 10px; }");
    highScoreLabel->setText("最高分: 0");
    highScoreLabel->setGeometry(20, 65, 120, 30);

    dashLabel = new QLabel(this);
    dashLabel->setFont(normalFont);
    dashLabel->setStyleSheet("QLabel { color: #00FF00; background-color: rgba(0,0,0,100); border-radius: 8px; padding: 3px 10px; }");
    dashLabel->setText("冲刺: 就绪！");
    dashLabel->setGeometry(20, 100, 120, 30);

    gameOverLabel = new QLabel(this);
    gameOverLabel->setFont(titleFont);
    gameOverLabel->setStyleSheet("QLabel { color: #FF4444; background-color: rgba(0,0,0,150); border-radius: 15px; padding: 20px 40px; }");
    gameOverLabel->setAlignment(Qt::AlignCenter);
    gameOverLabel->setText("游戏结束！\n酱板鸭被抓到了！\n\n\"我就知道这只鸭子跑不掉！\"");
    gameOverLabel->hide();

    tipLabel = new QLabel(this);
    tipLabel->setFont(tipFont);
    tipLabel->setStyleSheet("QLabel { color: #FFFFFF; background-color: rgba(0,0,0,100); border-radius: 8px; padding: 5px 15px; }");
    tipLabel->setAlignment(Qt::AlignCenter);
    tipLabel->setText("空格键=跳跃 | 右键=冲刺 | 逃跑吧酱板鸭！");
    tipLabel->setGeometry(width()/2 - 150, height() - 60, 300, 30);

    startButton = new QPushButton("开始游戏", this);
    startButton->setFont(titleFont);
    startButton->setStyleSheet("QPushButton { background-color: #FF6B35; color: white; border-radius: 15px; padding: 15px 50px; border: 3px solid #FF8C55; }"
                               "QPushButton:hover { background-color: #FF8C55; }");
    startButton->setGeometry(width()/2 - 100, height()/2 - 40, 200, 80);
    connect(startButton, &QPushButton::clicked, this, &GameWidget::onStartGame);

    restartButton = new QPushButton("重新开始", this);
    restartButton->setFont(titleFont);
    restartButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border-radius: 15px; padding: 15px 50px; border: 3px solid #66BB6A; }"
                                 "QPushButton:hover { background-color: #66BB6A; }");
    restartButton->setGeometry(width()/2 - 100, height()/2 + 100, 200, 80);
    restartButton->hide();
    connect(restartButton, &QPushButton::clicked, this, &GameWidget::onRestartGame);

    player = new Player(this);
    player->setGeometry(100, height() - GROUND_HEIGHT - 80, 80, 80);

    connect(gameTimer, &QTimer::timeout, this, &GameWidget::onGameUpdate);
    connect(scoreTimer, &QTimer::timeout, this, &GameWidget::updateScore);
    connect(dashCooldownTimer, &QTimer::timeout, this, &GameWidget::onDashCooldown);
}

GameWidget::~GameWidget()
{
}

void GameWidget::initGame()
{
    foreach (Obstacle *obs, obstacles) {
        delete obs;
    }
    obstacles.clear();

    score = 0;
    gameSpeed = 8;
    dashCooldown = 0;
    isGameOver = false;
    canDash = true;

    scoreLabel->setText("分数: 0");
    dashLabel->setText("冲刺: 就绪！");
    dashLabel->setStyleSheet("QLabel { color: #00FF00; background-color: rgba(0,0,0,100); border-radius: 8px; padding: 3px 10px; }");

    groundOffset = 0;
    bgOffset1 = 0;
    bgOffset2 = GAME_WIDTH;

    player->setGeometry(100, height() - GROUND_HEIGHT - 80, 80, 80);
    player->stopRunning();
    player->setDashing(false);
    player->update();
}

void GameWidget::onStartGame()
{
    initGame();
    startButton->hide();
    isGameRunning = true;
    player->startRunning();
    startGameLoop();
}

void GameWidget::onRestartGame()
{
    gameOverLabel->hide();
    restartButton->hide();
    initGame();
    isGameRunning = true;
    player->startRunning();
    startGameLoop();
}

void GameWidget::startGameLoop()
{
    gameTimer->start(30);
    scoreTimer->start(100);
}

void GameWidget::stopGameLoop()
{
    gameTimer->stop();
    scoreTimer->stop();
    dashCooldownTimer->stop();
}

void GameWidget::onGameUpdate()
{
    if (!isGameRunning || isGameOver) return;

    generateObstacle();

    foreach (Obstacle *obs, obstacles) {
        int currentSpeed = player->isDashing() ? gameSpeed * 2 : gameSpeed;
        obs->setSpeed(currentSpeed);
        obs->moveLeft();

        if (obs->x() < -100) {
            obs->reset();
        }
    }

    checkCollisions();

    groundOffset -= player->isDashing() ? gameSpeed * 2 : gameSpeed;
    if (groundOffset <= -50) groundOffset = 0;

    bgOffset1 -= (player->isDashing() ? gameSpeed * 2 : gameSpeed) / 2;
    bgOffset2 -= (player->isDashing() ? gameSpeed * 2 : gameSpeed) / 2;
    if (bgOffset1 <= -GAME_WIDTH) bgOffset1 = GAME_WIDTH;
    if (bgOffset2 <= -GAME_WIDTH) bgOffset2 = GAME_WIDTH;

    player->update();

    update();
}

void GameWidget::generateObstacle()
{
    static int generateCounter = 0;
    generateCounter++;

    if (generateCounter >= 60) {
        generateCounter = 0;

        int type = QRandomGenerator::global()->bounded(3);
        Obstacle *obs = new Obstacle(this);

        int groundY = height() - GROUND_HEIGHT;

        switch (type) {
        case 0:
            obs->setObstacleType(Obstacle::Type_Barrier);
            obs->setFixedSize(50, 60);
            obs->move(width() + 50, groundY - 60);
            break;
        case 1:
            obs->setObstacleType(Obstacle::Type_Flying);
            obs->setFixedSize(50, 40);
            obs->move(width() + 50, groundY - 120 - QRandomGenerator::global()->bounded(50));
            break;
        case 2:
            obs->setObstacleType(Obstacle::Type_Ground);
            obs->setFixedSize(55, 35);
            obs->move(width() + 50, groundY - 35);
            break;
        }

        obs->setSpeed(gameSpeed);
        obstacles.append(obs);
        obs->show();
    }
}

void GameWidget::checkCollisions()
{
    QRect playerRect = player->getCollisionRect();

    foreach (Obstacle *obs, obstacles) {
        QRect obsRect = obs->getCollisionRect();

        if (playerRect.intersects(obsRect)) {
            gameOver();
            return;
        }
    }
}

void GameWidget::updateScore()
{
    if (!isGameRunning || isGameOver) return;

    score += 1;
    scoreLabel->setText(QString("分数: %1").arg(score));

    if (score % 100 == 0 && gameSpeed < 15) {
        gameSpeed += 1;
    }
}

void GameWidget::gameOver()
{
    isGameRunning = false;
    isGameOver = true;
    stopGameLoop();

    player->stopRunning();

    if (score > highScore) {
        highScore = score;
        highScoreLabel->setText(QString("最高分: %1").arg(highScore));
    }

    QStringList memes = QStringList()
                        << "我就知道这只鸭子跑不掉！"
                        << "酱板鸭：终究还是被抓住了..."
                        << "逃跑失败！明天就上桌！"
                        << "这鸭子跑得还挺快，可惜没用！"
                        << "抓到了！今晚加菜！";

    gameOverLabel->setText(QString("游戏结束！\n酱板鸭被抓到了！\n\n%1\n\n最终得分: %2").arg(memes[QRandomGenerator::global()->bounded(memes.size())]).arg(score));
    gameOverLabel->move(width()/2 - 200, height()/2 - 120);
    gameOverLabel->show();

    restartButton->move(width()/2 - 100, height()/2 + 80);
    restartButton->show();
}

void GameWidget::onDashCooldown()
{
    dashCooldown -= 100;
    if (dashCooldown <= 0) {
        dashCooldown = 0;
        canDash = true;
        dashLabel->setText("冲刺: 就绪！");
        dashLabel->setStyleSheet("QLabel { color: #00FF00; background-color: rgba(0,0,0,100); border-radius: 8px; padding: 3px 10px; }");
        dashCooldownTimer->stop();
    } else {
        dashLabel->setText(QString("冲刺: %1秒").arg(dashCooldown / 1000));
    }
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Space) {
        if (isGameRunning && !isGameOver) {
            player->jump();
        }
        event->accept();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void GameWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        if (isGameRunning && !isGameOver && canDash) {
            player->setDashing(true);
            canDash = false;
            dashCooldown = DASH_COOLDOWN_TIME;
            dashLabel->setText(QString("冲刺: %1秒").arg(dashCooldown / 1000));
            dashLabel->setStyleSheet("QLabel { color: #FF6600; background-color: rgba(0,0,0,100); border-radius: 8px; padding: 3px 10px; }");
            dashCooldownTimer->start(100);

            QTimer *dashEndTimer = new QTimer(this);
            dashEndTimer->setSingleShot(true);
            connect(dashEndTimer, &QTimer::timeout, this, [this, dashEndTimer]() {
                player->setDashing(false);
                dashEndTimer->deleteLater();
            });
            dashEndTimer->start(500);
        }
        event->accept();
    } else {
        QWidget::mousePressEvent(event);
    }
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    drawGround(painter);

    foreach (Obstacle *obs, obstacles) {
        obs->raise();
    }
    player->raise();

    QWidget::paintEvent(event);
}

void GameWidget::drawBackground(QPainter &painter)
{
    QLinearGradient gradient(0, 0, 0, height());
    gradient.setColorAt(0, QColor(135, 206, 235));
    gradient.setColorAt(0.7, QColor(255, 218, 185));
    gradient.setColorAt(1, QColor(210, 180, 140));
    painter.fillRect(0, 0, width(), height(), gradient);

    painter.setBrush(QBrush(QColor(180, 220, 180)));
    painter.setPen(Qt::NoPen);
    QPainterPath hillPath1;
    hillPath1.moveTo(bgOffset1, height() - GROUND_HEIGHT);
    hillPath1.quadTo(bgOffset1 + 200, height() - GROUND_HEIGHT - 150, bgOffset1 + 400, height() - GROUND_HEIGHT);
    hillPath1.closeSubpath();
    painter.drawPath(hillPath1);

    QPainterPath hillPath2;
    hillPath2.moveTo(bgOffset2, height() - GROUND_HEIGHT);
    hillPath2.quadTo(bgOffset2 + 200, height() - GROUND_HEIGHT - 120, bgOffset2 + 400, height() - GROUND_HEIGHT);
    hillPath2.closeSubpath();
    painter.drawPath(hillPath2);

    painter.setBrush(QBrush(QColor(100, 180, 100)));
    QPainterPath cloudPath1;
    cloudPath1.moveTo((bgOffset1 + 100) % (GAME_WIDTH * 2), 80);
    cloudPath1.addEllipse((bgOffset1 + 100) % (GAME_WIDTH * 2), 70, 60, 40);
    cloudPath1.addEllipse((bgOffset1 + 130) % (GAME_WIDTH * 2), 60, 50, 35);
    cloudPath1.addEllipse((bgOffset1 + 160) % (GAME_WIDTH * 2), 70, 55, 38);
    painter.drawPath(cloudPath1);

    QPainterPath cloudPath2;
    cloudPath2.moveTo((bgOffset2 + 300) % (GAME_WIDTH * 2), 120);
    cloudPath2.addEllipse((bgOffset2 + 300) % (GAME_WIDTH * 2), 110, 50, 35);
    cloudPath2.addEllipse((bgOffset2 + 330) % (GAME_WIDTH * 2), 100, 45, 30);
    cloudPath2.addEllipse((bgOffset2 + 360) % (GAME_WIDTH * 2), 110, 48, 33);
    painter.drawPath(cloudPath2);
}

void GameWidget::drawGround(QPainter &painter)
{
    int groundY = height() - GROUND_HEIGHT;

    painter.setBrush(QBrush(QColor(139, 90, 43)));
    painter.setPen(Qt::NoPen);
    painter.drawRect(0, groundY, width(), GROUND_HEIGHT);

    painter.setBrush(QBrush(QColor(160, 110, 50)));
    for (int i = 0; i < width() + 50; i += 50) {
        int x = (i + groundOffset) % (width() + 50) - 25;
        painter.drawRect(x, groundY, 48, 5);
    }

    painter.setBrush(QBrush(QColor(34, 139, 34)));
    painter.drawRect(0, groundY, width(), 15);

    painter.setBrush(QBrush(QColor(50, 180, 50)));
    for (int i = 0; i < width() + 30; i += 30) {
        int x = (i + groundOffset * 2) % (width() + 30) - 15;
        static const QPoint grassPoints1[] = {
            QPoint(0, groundY), QPoint(5, groundY - 15), QPoint(10, groundY)
    };
    static const QPoint grassPoints2[] = {
        QPoint(10, groundY), QPoint(15, groundY - 12), QPoint(20, groundY)
};
static const QPoint grassPoints3[] = {
    QPoint(20, groundY), QPoint(25, groundY - 18), QPoint(30, groundY)
};

painter.save();
painter.translate(x, groundY);
painter.drawPolygon(grassPoints1, 3);
painter.drawPolygon(grassPoints2, 3);
painter.drawPolygon(grassPoints3, 3);
painter.restore();
}
}
