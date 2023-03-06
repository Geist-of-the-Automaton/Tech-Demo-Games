#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTimer>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPoint>
#include <QPointF>
#include <QFile>
#include <QThread>
#include <QCursor>

#include <iostream>
#include <time.h>
#include <chrono>
#include <vector>
#include <list>
#include <string>

#include <rope.h>
#include <limb.h>


using std::max;
using std::min;
using std::cout;
using std::endl;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::chrono::milliseconds;
using std::vector;
using std::list;
using std::pair;
using std::string;
using std::to_string;
using std::reverse;

const float pi = 3.14159265359;
const int WIDTH = 1280, HEIGHT = 720; //320x240
const int FPS = 60;
const float fFPS = static_cast<float>(FPS);
const float ifFPS = 1.0 / fFPS;
const int mapBottom = 662;
static float gravity = 800.0;
static float expForce = 1000.0;
static float bGrav = 4.0;
static float groundFrict = 0.95;
static float airFrict = 0.99;
const float yImpulse = -500.0;
const float xImpulse = 250.0;
const float xAcc = 900 / fFPS;
const float yBounceCoef = 1.0 / 3.0;
const float xBounceCoef = 1.0 / 1.5;
enum shotType {automatic, shotgun, bouncer, launcher, rocket, flamethrower, laser, silver, ropeShot, shotParticle, expParticle};
const int numWeapons = 11;
const int cooldown[numWeapons] = {2, 40, 60, 50, 80, 2, 1, 500, 0, 0, 0};
const float speeds[numWeapons] = {1200.0f / fFPS, 800.0f / fFPS, 800.0f / fFPS, 450.0f / fFPS, 200.0f / fFPS, 500.0f / fFPS, 400.0f / fFPS, 1000.0f / fFPS, 400.0f / fFPS, 50.0f / fFPS, 200.0f / fFPS};

const int imgOffsets[4] = {192, 364, 165, -100};

static QPoint targetPt;

struct weaponIKs {
    QPoint leftH, rightH, fireLoc, fixedFireLoc;
    bool anchorOffset = false;
};

typedef pair<QPoint, int> Explosion;

struct projectile {
    float x, y, xInc, yInc, grav, angle;
    shotType type;

    projectile(float px, float py, QPoint end, shotType TYPE) {
        grav = 0;
        x = px;
        y = py;
        type = TYPE;
        angle = atan2(py - end.y(), px - end.x());
        float speed = speeds[type];
        if (type >= shotParticle) {
            speed += static_cast<float>(fastrand() % 50) * ifFPS;
            if (type == expParticle)
                type = shotParticle;
        }
        else if (type != laser && type != ropeShot)
             speed += static_cast<float>(fastrand() % 10) / 10.0;
        xInc = cos(angle) * speed;
        yInc = sin(angle) * speed;
    }

    void advance() {
        x -= xInc;
        y -= yInc;
        if (type == shotParticle || type == launcher || type == bouncer || type == ropeShot) {
            grav += bGrav * ifFPS;
            y += grav;
        }
        else if (type == rocket) {
            QPoint to = targetPt;
            float dist = sqrt(pow(to.x() - x, 2) + pow(to.y() - y, 2));
            QPoint toOld = QPoint(x - xInc * dist, y - yInc * dist);
            float curvingFactor = 1.0 / sqrt(dist);
            to = (curvingFactor * to + toOld) / (1.0 + curvingFactor);
            angle = atan2(y - to.y(), x - to.x());
            float speed = speeds[rocket];
            xInc = cos(angle) * speed;
            yInc = sin(angle) * speed;
        }
        else if (type == flamethrower) {
            grav += bGrav * bGrav * ifFPS;
            y += grav;
        }
    }
};


typedef pair<projectile, int> Particle;


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE


static long long t;

static void logTime() {
    t = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
}

static void getTime() {
    t = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - t;
    cout << t << endl;
}

static void getTimeSilent() {
    t = duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - t;
}

//static long long getTime(long long initial = 0) {
//    return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count() - initial;
//}

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
    static void hairSolver(vector <Rope> *vec, int start, int end);
    static void ropeSolver(Rope *rope);

public slots:
    void nextFrame();

private:
    void expAffect(QPoint where);
    pair <bool, QPoint> raycast(QPoint from, QPoint to);
    int onSegment(QPoint p, QPoint q, QPoint r);
    int orient(QPoint p, QPoint q, QPoint r);
    int doIntersect(QPoint p1, QPoint q1, QPoint p2, QPoint q2);

    Ui::MainWindow *ui;
    float xSpeed, ySpeed;
    float px, py, lastX, lastY;
    QTimer redraw;
    Qt::Key lastKey;
    bool hasDJ;
    shotType weapon;
    vector <projectile> projectiles;
    bool onGround;
    QPixmap lA, rA, bg, bg1, bg2, bg3, target;
    int pH, pW, pH2, pW2;
    QPixmap projSprites[numWeapons];
    vector <Explosion> explosions;
    vector <QPixmap> expSeq;
    vector <Particle> particles, otherParticles;
    vector <QPixmap> particleTrails;
    vector <Rope> hairs;
    bool keyDown;
    int timers[numWeapons] = {0};
    bool mousePressed = false;
    bool rightBtn = false;
    bool LorR;
    float avgFPS = 0.0;
    int lowFPS = 10000000;
    int highFPS = 0;
    int frameCnt = 0;
    int expDistance = 0;
    float inbetweening;
    QPoint lf, rf, lastTar, nextTar, pt;

    void process();
    QPointF asleg, aeleg, bsleg, beleg, csleg, celeg, dleg, eleg, rtleg, ltleg;
    float interpR = 1.0;
    float offset = 0.0;
    float adderleg = 0.01;
    float scale = 4.0;
    bool whichBtn, whichLeg, sameDir, flipper = false;
    QPixmap body;

    QPointF a, b, as, ae, bs, be, cs, ce, lik, rik, fixedFLoc;
    float interpC = 0.0;
    float adder = 0.0;
    QPixmap weaponPix[5];
    weaponIKs wIKs[5];
    int wIndex = 0;
    vector <Limb> limbs;
    vector <QRect> bounds;
    bool reversed = true;
    int throwingStatus = 0;

    bool showBounds = false;

    QPoint bagLoc;
    QPixmap bagImg;
    bool bagPlaced = false;
    vector <vector <unsigned char>> record;
    Rope lead;
    QPointF lastPt;
    //QImage testRecord;
    bool climbing = false;
    int whichSeg;

    vector <QPoint> testPts;
    vector <QPointF> shotPts;
    vector <QPointF> bagArc;
};
#endif // MAINWINDOW_H
