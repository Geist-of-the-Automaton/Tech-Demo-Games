#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QImage>
#include <QPainter>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPoint>
#include <QTimer>
#include <QDir>
#include <QFont>
#include <QtMultimedia/QAudio>
#include <QtMultimedia/QMediaPlayer>
#include <QtMultimedia/QSoundEffect>

#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <time.h>
#include <chrono>
using std::vector;
using std::string;
using std::to_string;
using std::cout;
using std::endl;
using std::unordered_map;
using std::min;
using std::max;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;

const int hoff = 10;
const float vSep = 1.75;
const int voff = 200;
const int movement = 50;
const int starSizes = 5;
const int numStarsPerSize = 10;
const int playerSpeed = 12;
const int baseFireRate = 10;
const int maxFireRate = 5;
const int maxPlayerSpeed = 12;
const int baseHP = 1000;

const int fps = 90;
const int scale = 3;

const float pi = 3.14159265359;
const float deg2rad = pi / 180.0;

enum drawType {title, game, scores, settings, choose, pause, gameover, trans, pauseBack};
enum projType {laser, bouncer, rocket, asteroid};
//int arrDmgs[] = {10, 5, 100, 500};

struct projectile {
    float x, y, xinc, yinc;
    projType type;
    bool friendly;

    projectile(float X, float Y, projType t, bool fromPlayer = true) {
        x = X;
        y = Y;
        type = t;
        friendly = fromPlayer;
    }

    void advance() {
        y -= 10;
    }
};

enum enemyType {gunship, crasher, eliteGunship, eliteCrasher, megaship, dropship, blocker};
enum moveType {left, right};

struct enemy {
    QPoint loc;
    int hp;
    enemyType type;
    int cooldown;
    int movesVals[4] = {0, 0, 0, 0};

    enemy(QPoint l, enemyType t) {
        loc = l;
        type = t;
    }

    int calcMove(int depth) {
        if (depth == -1)
            return 0;
        if (type == blocker) {

        }
    }
};

static long long t;

static void logTime(long long off = 0) {
    t = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - off;
}

static void getTime() {
    cout << duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - t << endl;
}

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void nextFrame();

private:

    Ui::MainWindow *ui;
    QSoundEffect menuTheme, menuOver, menuSelect, menuBack;
    int lastOver = -1;


    QImage bg, wasdOn, wasdOff;
    QPixmap blackBg, mlister, slister, titleImg, bgTitle, tplane, laserImg, pauseImg, bgPause, wasdImg, wasdSlider, clearScores;
    QTimer timer;
    drawType type;
    vector <QString> texts;
    float bgtOff;
    vector <int> outs;
    vector <QString> scoreTexts;
    qreal transDir = 0.02;
    qreal cover = 0.0;
    int over, shift;
    float tinc;
    unordered_map <drawType, vector<QString>> textMaps;
    vector <vector <QPointF>> stars;
    vector <QPixmap> starImgs;
    vector <QPixmap> cntDownF;
    vector <QPixmap> cntDownB;
    QPoint playerLoc;
    vector <QPixmap> ships;
    int up, down, left, right;
    bool isFiring, isFiringSec, usesMouse, frameless;
    vector <projectile> projectiles;
    int cooldown, gun, wasdSliderOff;
    long long cntDown;
    vector <vector <QPoint>> gunLocs;
    vector <enemy> enemies;

    int speedAdder = 0;
    int hpAdder = 0;
    int firerateAdder = 0;
    int boosts = 0;
    int lives = 0;
    int dmgAdder = 0;
    int rockets = 0;
    int shields = 0;
    int shieldHp = 0;
    bool hasPara = false;
    int playerShip;
    int hp = baseHP;
    int lvl = 0;

};
#endif // MAINWINDOW_H

