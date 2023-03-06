#include "limb.h"

Limb::Limb(int numSegs, float len, QPointF lock, bool anchored) {
    anchor = lock;
    numSegments = numSegs - 1;
    float l = len / static_cast<float>(numSegments);
    if (l < 2.0)
        l = 2.0;
    totalLength = l * static_cast<float>(numSegments);
    first.length = final.length = l;
    segments = vector <limbSeg> (numSegments, l);
}

void Limb::solve(QPointF to) {
    segments[0].solve(to);
    for (int i = 1; i < numSegments; ++i)
        segments[i].solve(segments[i - 1].A);
    segments[numSegments - 1].solve(anchor);
    for (int i = segments.size() - 2; i >= 0; --i) {
        segments[i].solve(segments[i + 1].A);
        segments[i + 1].B = segments[i].A;
    }
    first.B = segments[numSegments - 1].A;
    first.A = anchor;
    final.B = segments[0].A;
    QPointF pMag = to - final.B;
    if (pMag.x() != 0.0 || pMag.y() != 0.0)
        pMag = pMag / sqrt(pow(pMag.x(), 2) + pow(pMag.y(), 2));
    QPointF dir = pMag * final.length;
    final.A = final.B + dir;
}
