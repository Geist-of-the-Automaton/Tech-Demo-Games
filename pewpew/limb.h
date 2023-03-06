#ifndef LIMB_H
#define LIMB_H


#include <QPoint>
#include <vector>
#include <iostream>
#include <QColor>
#include <QLine>
using std::cout;
using std::endl;
using std::vector;

struct limbSeg {
    float length, angle;
    QPointF A, B;

    limbSeg(float len = 2.0) {
        length = len;
        angle = 0.0;
        A = QPointF(0.0, 0.0);
        B = QPointF(2.0, 0.0);
    }

    void solve(QPointF target) {
        QPointF pMag = target - A;
        if (pMag.x() != 0.0 || pMag.y() != 0.0)
            pMag = pMag / sqrt(pow(pMag.x(), 2) + pow(pMag.y(), 2));
        if (pMag.x() != 0.0)
            angle = atan(pMag.y() / pMag.x());
        else
            angle = 0.0;
        QPointF dir = pMag * -length;
        A = target + dir;
        float dx = length * cos(angle);
        float dy = length * sin(angle);
        B = QPointF(A.x() + dx, A.y() + dy);
        B = target;
    }

    QLine line() {
        return QLine(A.toPoint(), B.toPoint());
    }
};


class Limb {

public:
    int numSegments, totalLength;
    vector <limbSeg> segments;
    QPointF anchor, sought;
    limbSeg first, final;
    float width;
    QColor color;

    Limb(int numSegs, float len = 0.0, QPointF lock = QPointF(0.0, 0.0), bool anchored = true);
    void solve(QPointF to);

};

#endif // LIMB_H
