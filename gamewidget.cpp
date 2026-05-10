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
    setFocusPolicy(Qt::StrongFocus);

    QFont titleFont("Microsoft YaHei", 24, QFont::Bold);
    QFont normalFont("Microsoft YaHei", 14);
    QFont scoreFont("Microsoft YaHei", 18, QFont::Bold);
    QFont tipFont("Microsoft YaHei", 12);

    scoreLabel = new QLabel(this);
    scoreLabel->setFont(scoreFont);
    scoreLabel->setStyleSheet("QLabel { color: #FFFFFF; background-color: rgba(0,0,0,100); border-radius: 10px; padding: 5px 15px; }");
    scoreLabel->setText("分数: 0");
    scoreLabel->setGeometry(20, 20, 220, 40);

    highScoreLabel = new QLabel(this);
    highScoreLabel->setFont(normalFont);
    highScoreLabel->setStyleSheet("QLabel { color: #FFD700; background-color: rgba(0,0,0,100); border-radius: 8px; padding: 3px 10px; }");
    highScoreLabel->setText("最高分: 0");
    highScoreLabel->setGeometry(20, 65, 200, 30);

    dashLabel = new QLabel(this);
    dashLabel->setFont(normalFont);
    dashLabel->setStyleSheet("QLabel { color: #00FF00; background-color: rgba(0,0,0,100); border-radius: 8px; padding: 3px 10px; }");
    dashLabel->setText("冲刺: 就绪！");
    dashLabel->setGeometry(20, 100, 120, 30);

    gameOverLabel = new QLabel(this);
    gameOverLabel->setFont(titleFont);
    gameOverLabel->setStyleSheet("QLabel { color: #FF4444; background-color: rgba(0,0,0,150); border-radius: 15px; padding: 30px 50px; }");
    gameOverLabel->setAlignment(Qt::AlignCenter);
    gameOverLabel->setText(QString::fromUtf8("游戏结束！\n\n你是那只酱板鸭？\n\n\"我就知道这只鸭子跑不掉！\""));
    gameOverLabel->hide();

    tipLabel = new QLabel(this);
    tipLabel->setFont(tipFont);
    tipLabel->setStyleSheet("QLabel { color: #FFFFFF; background-color: rgba(0,0,0,100); border-radius: 8px; padding: 5px 15px; }");
    tipLabel->setAlignment(Qt::AlignCenter);
    tipLabel->setText("空格键=跳跃 | 右键=冲刺 | 逃跑吧酱板鸭！");
    tipLabel->setGeometry(width()/2 - 150, height() - 60, 300, 30);

    startButton = new QPushButton(QString::fromUtf8("开始游戏"), this);
    startButton->setFont(titleFont);
    startButton->setStyleSheet("QPushButton { background-color: #FF6B35; color: white; border-radius: 15px; padding: 20px 50px; border: 3px solid #FF8C55; text-align: center; min-height: 80px; }"
                               "QPushButton:hover { background-color: #FF8C55; }");
    startButton->setGeometry(width()/2 - 120, height()/2, 240, 80);
    connect(startButton, &QPushButton::clicked, this, &GameWidget::onStartGame);

    restartButton = new QPushButton(QString::fromUtf8("重新开始"), this);
    restartButton->setFont(titleFont);
    restartButton->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; border-radius: 15px; padding: 20px 40px; border: 3px solid #66BB6A; text-align: center; min-height: 70px; }"
                                 "QPushButton:hover { background-color: #66BB6A; }");
    restartButton->setGeometry(width()/2 - 120, height()/2 + 120, 240, 70);
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

    int playerY = height() - GROUND_HEIGHT - 80;
    player->move(100, playerY);
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

        if (obs->x() < -200) {
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

    const int MIN_SAFE_GAP = 240;
    const int DOUBLE_OBSTACLE_GAP = 200;

    double difficultyFactor = 1.0 + score / 1500.0;
    int baseInterval = 52;
    int minInterval = qMax(28, baseInterval - (int)(score / 120));
    int interval = minInterval + QRandomGenerator::global()->bounded(minInterval / 2 + 1);

    if (generateCounter >= interval) {
        generateCounter = 0;

        int groundY = height() - GROUND_HEIGHT;
        int startX = width() + 80;
        bool canGenerate = true;

        foreach (Obstacle *obs, obstacles) {
            int obsRightEdge = obs->x() + obs->width();
            int gap = startX - obsRightEdge;

            if (gap < MIN_SAFE_GAP && obs->x() < width() + 350) {
                canGenerate = false;
                break;
            }
        }

        if (canGenerate) {
            Obstacle *obs1 = new Obstacle(this);
            obs1->setFixedSize(160, 160);
            obs1->setPosition(startX, groundY - 160);
            obs1->setSpeed(gameSpeed * difficultyFactor);
            obstacles.append(obs1);
            obs1->show();

            if (QRandomGenerator::global()->bounded(4) == 0 && score > 250) {
                int secondObsX = startX + 160 + DOUBLE_OBSTACLE_GAP;
                bool canGenerateSecond = true;

                foreach (Obstacle *obs, obstacles) {
                    if (obs == obs1) continue;
                    int obsRightEdge = obs->x() + obs->width();
                    int gap = secondObsX - obsRightEdge;

                    if (gap < MIN_SAFE_GAP && obs->x() < width() + 450) {
                        canGenerateSecond = false;
                        break;
                    }
                }

                if (canGenerateSecond) {
                    Obstacle *obs2 = new Obstacle(this);
                    obs2->setFixedSize(160, 160);
                    obs2->setPosition(secondObsX, groundY - 160);
                    obs2->setSpeed(gameSpeed * difficultyFactor);
                    obstacles.append(obs2);
                    obs2->show();
                }
            }
        }
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

    if (score % 50 == 0 && gameSpeed < 18) {
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
                        << "这鸭子跑得还挺快，可惜没用！";

    gameOverLabel->setText(QString::fromUtf8("游戏结束！\n\n你是那只酱板鸭？\n\n%1\n\n最终得分: %2").arg(memes[QRandomGenerator::global()->bounded(memes.size())]).arg(score));
    gameOverLabel->move(width()/2 - 220, height()/2 - 150);
    gameOverLabel->show();

    restartButton->move(width()/2 - 120, height()/2 + 120);
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

bool GameWidget::event(QEvent *event)
{
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Space) {
            if (isGameRunning && !isGameOver) {
                player->jump();
            }
            event->accept();
            return true;
        }
    }
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::RightButton) {
            if (isGameRunning && !isGameOver) {
                player->setDashing(true);
                dashLabel->setText("冲刺中！");
                dashLabel->setStyleSheet("QLabel { color: #FF6600; background-color: rgba(0,0,0,100); border-radius: 8px; padding: 3px 10px; }");

                QTimer *dashEndTimer = new QTimer(this);
                dashEndTimer->setSingleShot(true);
                connect(dashEndTimer, &QTimer::timeout, this, [this, dashEndTimer]() {
                    player->setDashing(false);
                    dashLabel->setText("冲刺: 就绪！");
                    dashLabel->setStyleSheet("QLabel { color: #00FF00; background-color: rgba(0,0,0,100); border-radius: 8px; padding: 3px 10px; }");
                    dashEndTimer->deleteLater();
                });
                dashEndTimer->start(500);
            }
            event->accept();
            return true;
        }
    }
    return QWidget::event(event);
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    drawBackground(painter);
    drawGround(painter);

    static QImage obstacleImage;
    static bool obstacleLoaded = false;
    if (!obstacleLoaded) {
        bool loaded = obstacleImage.load(":/images/baihu.jpg");
        if (loaded && !obstacleImage.isNull()) {
            obstacleImage = obstacleImage.scaled(160, 160, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        } else {
            qDebug() << "Error: Failed to load obstacle image: :/images/baihu.jpg";
            obstacleImage = QImage(160, 160, QImage::Format_ARGB32);
            obstacleImage.fill(Qt::red);
        }
        obstacleLoaded = true;
    }

    foreach (Obstacle *obs, obstacles) {
        int offsetX = (obs->width() - obstacleImage.width()) / 2;
        int offsetY = obs->height() - obstacleImage.height();
        painter.drawImage(obs->x() + offsetX, obs->y() + offsetY, obstacleImage);
    }

    player->raise();

    QWidget::paintEvent(event);
}

void GameWidget::drawBackground(QPainter &painter)
{
    static QImage bgImage;
    static bool bgLoaded = false;

    if (!bgLoaded) {
        bool loaded = bgImage.load(":/images/xueshan.png");
        if (loaded) {
            bgImage = bgImage.scaled(width() * 2, height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        } else {
            bgImage = QImage(width() * 2, height(), QImage::Format_ARGB32);
            QPainter p(&bgImage);
            QLinearGradient gradient(0, 0, 0, height());
            gradient.setColorAt(0, QColor(135, 206, 235));
            gradient.setColorAt(0.5, QColor(255, 255, 255));
            gradient.setColorAt(1, QColor(200, 220, 255));
            p.fillRect(0, 0, width() * 2, height(), gradient);

            p.setBrush(QBrush(QColor(255, 255, 255, 200)));
            p.setPen(Qt::NoPen);
            for (int j = 0; j < 2; j++) {
                int offsetX = j * width();
                for (int i = 0; i < 5; i++) {
                    int x = 100 + i * 180 + offsetX;
                    int y = 50 + (i % 3) * 40;
                    p.drawEllipse(x, y, 60, 40);
                    p.drawEllipse(x + 20, y - 10, 50, 35);
                    p.drawEllipse(x + 40, y, 55, 38);
                }
            }
        }
        bgLoaded = true;
    }

    painter.drawImage(bgOffset1, 0, bgImage);
    painter.drawImage(bgOffset1 + width(), 0, bgImage);
    painter.drawImage(bgOffset2, 0, bgImage);
    painter.drawImage(bgOffset2 + width(), 0, bgImage);
}

void GameWidget::drawGround(QPainter &painter)
{
    Q_UNUSED(painter);
}
