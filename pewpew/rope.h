#ifndef ROPE_H
#define ROPE_H

#include <QColor>
#include <QPointF>
#include <vector>
#include <iostream>

using std::endl;
using std::cout;

using std::vector;

static unsigned int g_seed = 0;

inline int fastrand() {
  g_seed = 214013 * g_seed + 2531011;
  return (g_seed >> 16) & 0x7FFF;
}

struct RopeSegment {
    QPointF posNow, posOld;

    RopeSegment(QPointF pt) {
        posNow = posOld = pt;
    }
};

struct Rope {
    QPointF anchor, tracking;
    bool anchored, isHair = true;
    float segLength, width, gravity, dampening;
    QColor color;
    short numSegments, maxSegments;
    vector <RopeSegment> segments;
    vector <QPoint> anchors;

    // 0.02, 0.037 rope?
    // 0.2-0.4, 0.037-0.04 hair;
    Rope(int segs = 1, float segLen = 2.0, float w = 1.0, QColor clr = 0xFF000000, float grav = 0.02, float damp = 0.007, bool attached = false) {
        numSegments = segs;
        segLength = segLen;
        width = w;
        color = clr;
        gravity = grav;
        dampening = damp;
        anchored = attached;
        segments = vector <RopeSegment> (numSegments, QPointF());
    }

    void solve() {
        QPointF grav = QPointF(0.0, gravity);
        float x = segments[1].posNow.x();
        for (int i = 1; i < numSegments; ++i) {
            segments[i].posNow = QPointF(x * (dampening) + segments[i].posNow.x() * (1.0 - dampening), segments[i].posNow.y());
            x = segments[i].posNow.x();
            QPointF velocity = (segments[i].posNow - segments[i].posOld) * (1.0 - dampening);
            segments[i].posOld = segments[i].posNow;
            segments[i].posNow += velocity;
            segments[i].posNow += grav;// * Time.fixedDeltaTime;
        }
        // maybe make iterations based on some constant * the number of segments
        int iters = isHair ? numSegments * sqrt(numSegments) : 2 * ((static_cast<float>(numSegments) + 5.0) / log(numSegments + 5.0)) - 14;
        for (int j = 0; j < iters; ++j) {
            segments[0].posNow = tracking;
            if (anchored)
                segments[numSegments - 1].posNow = anchor;
            for (int i = 0; i < numSegments - 1; ++i) {
                QPointF diff = segments[i].posNow - segments[i + 1].posNow;
                float dist = sqrt(diff.x() * diff.x() + diff.y() * diff.y());
                float error = abs(dist - segLength);
                if (dist < segLength)
                    diff = -diff;
                diff /= dist;
                diff *= error;
                if (i != 0) {
                    diff *= 0.5;
                    segments[i].posNow -= diff;
                }
                segments[i + 1].posNow += diff;
            }
        }
    }

    void pushAnchor(QPoint pt) {
        anchors.push_back(tracking.toPoint());
        tracking = pt;
        solve();
    }

    void popAnchor() {
        if (anchors.size() == 1)
            return;
        tracking = anchors.back();
        anchors.pop_back();
        solve();
    }

    void pushFront(QPoint pt) {
        anchors.insert(anchors.begin() + 2, pt);
    }

};
#endif // ROPE_H
