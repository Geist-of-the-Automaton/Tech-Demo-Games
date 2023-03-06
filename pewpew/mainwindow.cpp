#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->statusbar->hide();
    setFixedSize(WIDTH, HEIGHT);
    //qi = QImage(WIDTH, HEIGHT, QImage::Format_ARGB32_Premultiplied);
    //qi.fill(0xFFFFFFFF);
    inbetweening = 1.0;
    lastTar = QPoint(rand() % WIDTH, rand() % HEIGHT);
    nextTar = QPoint(rand() % WIDTH, rand() % HEIGHT);
    xSpeed = ySpeed = 0.0;
    px = 0;
    //py = -7338;
    //xSpeed = 1400;
    hasDJ = true;
    projectiles.reserve(200);
    weapon = automatic;
    QImage temp;
    temp.load("LeftActionBaseNew.png");
    temp = temp.scaledToWidth(temp.width() * 2);
    lA = QPixmap::fromImage(temp);
    temp.mirror(true, false);
    rA = QPixmap::fromImage(temp);
    pH = temp.height();
    pW = temp.width();
    pH2 = pH / 2;
    pW2 = pW / 2;
    temp.load("LeftActionHairMap.png");
    for (int i = 0; i < temp.width(); ++i) {
        int I = i * 2 + px - pW2;
        for (int j = 0; j < temp.height(); ++j) {
            if (temp.pixel(i, j) == 0xFFFF00FF) {
                int J = j * 2 + py - pH;
                int k = 1;
                while (j + k < temp.height() && (temp.pixel(i, j + k) == 0xFF00FF00 || temp.pixel(i, j + k) == 0xFFFF00FF))
                    ++k;
                float hgadd = static_cast<float>(rand() % 3) / 20.0 - 0.05;
                float hdadd = static_cast<float>(rand() % 3) / 20.0 - 0.05;
                int color = rand() % 50;
                hairs.push_back(Rope(3 * k / 4, 1.0, 2.0, QColor(color, color, color), 0.2 + hgadd, 0.1 + hdadd));
                hairs.back().tracking = QPointF(I, J);
                color = rand() % 25;
                hairs.push_back(Rope(2 * k, 1.0, 2.0, QColor(color, color, color), 0.2 + hgadd, 0.1 + hdadd));
                hairs.back().tracking = QPointF(I, J);
            }
        }
    }
    //temp.load("bg.png");
    //bg = QPixmap::fromImage(temp.copy(0, 0, 1280, 720));
    bg.load("bg0nu2.png");
    bg1.load("bg1nu.png");
    bg2.load("bg2nu555.png");
    bg3.load("bg3nu.png");
    for (int i = 0; i < numWeapons; ++i) {
        int size = 3 * (i + 1);
        temp = QImage(size, size, QImage::Format_ARGB32_Premultiplied);
        temp.fill(0xFF222222);
        projSprites[i] = QPixmap::fromImage(temp);
    }
    temp.load("slug.png");
    projSprites[0] = QPixmap::fromImage(temp);
    temp = temp.scaledToWidth(temp.width() * 2, Qt::SmoothTransformation);
    projSprites[1] = QPixmap::fromImage(temp);
    temp = temp.scaledToWidth(temp.width() * 2, Qt::SmoothTransformation);
    projSprites[2] = QPixmap::fromImage(temp);
    temp.load("bomb.png");
    projSprites[3] = QPixmap::fromImage(temp.scaledToWidth(temp.width() * 0.75));
    temp.load("rocket.png");
    projSprites[4] = QPixmap::fromImage(temp);
    temp.load("laserSight.png");
    projSprites[5] = QPixmap::fromImage(temp);
    projSprites[6] = QPixmap::fromImage(temp);
    projSprites[8] = QPixmap();
    target.load("target.png");
    int i = 0;
    QPixmap qpm;
    expDistance = 0;
    while (true) {
        QFile qf(("exp" + std::to_string(i) + "nu.png").c_str());
        if (qf.exists()) {
            qpm.load(qf.fileName());
            expSeq.push_back(qpm);
            expDistance = max(expDistance, max(qpm.width(), qpm.height()) / 2);
            ++i;
        }
        else
            break;
    }
    for (int i = 1; i <= 255; i += 2) {
        int size = log10(i * 255 / (256 - i)) + 1;
        temp = QImage(size, size, QImage::Format_ARGB32_Premultiplied);
        temp.fill(QColor(i, i / 2, 0));
        particleTrails.push_back(QPixmap::fromImage(temp));
    }


    srand(time(NULL));
    setMinimumSize(1280, 720);

    int segs = 2;
    int length = 27;
    for (int i = 0; i < 2; ++i) {
        //ropes.push_back(rope(segs, length, QPointF(473 + 4, 110), true));
        limbs.push_back(Limb(segs, length, QPointF(-5, -pH2 + 10), true));
        limbs[i].color = i == 0 ? 0xFFAAAAAA : 0xFFC2C2C2;
        limbs[i].width = 5.0;
    }
    asleg = limbs[0].anchor + QPointF(limbs[0].segments[0].length * 0.75, limbs[0].segments[0].length * 1.85);
    aeleg = limbs[0].anchor + QPointF(-limbs[0].segments[0].length * 0.25, limbs[0].segments[0].length * 2);
    bsleg = limbs[0].anchor + QPointF(limbs[0].segments[0].length * 1.0, limbs[0].segments[0].length * 1.5);
    beleg = limbs[0].anchor + QPointF(-limbs[0].segments[0].length * 0.55, limbs[0].segments[0].length * 1.95);
    csleg = limbs[0].anchor + QPointF(-limbs[0].segments[0].length * 0.125, limbs[0].segments[0].length * 1.75);
    celeg = limbs[0].anchor + QPointF(-limbs[0].segments[0].length * 0.25, limbs[0].segments[0].length * 2.0);
    dleg = limbs[0].anchor + QPointF(0.0, -100 * limbs[0].segments[0].length);
    eleg = QPointF(3, -pH2 + 13);

    segs = 2;
    length = 17;
    for (int i = 0; i < 2; ++i) {
        limbs.push_back(Limb(segs, length, QPointF(-1, -(pH2 + pH) / 2 + 5), true));
        limbs[i + 2].color = i == 0 ? 0xFF777777 : 0xFF888888;
        limbs[i + 2].width = 5;
    }
    as = limbs[2].anchor + QPointF(-limbs[2].segments[0].length * 0.55, limbs[2].segments[0].length * 1.825);
    bs = limbs[2].anchor + QPointF(limbs[2].segments[0].length * 0.0625, limbs[2].segments[0].length * 1.975);
    ae = limbs[2].anchor + QPointF(-limbs[2].segments[0].length, limbs[2].segments[0].length * 0.125);
    be = limbs[2].anchor + QPointF(-limbs[2].segments[0].length * 0.25, limbs[2].segments[0].length * 1.75);
    cs = limbs[2].anchor + QPointF(-limbs[2].segments[0].length * 0.75, -limbs[2].segments[0].length * 1.75);
    ce = limbs[2].anchor + QPointF(-limbs[2].segments[0].length * 0.75, -limbs[2].segments[0].length * 0.25);

    QString names[5] = {"automatic", "shotgun", "bouncer", "launcher", "rocketGun"};
    int imgScale = 3;
    for (int i = 0; i < 5; ++i) {
        QImage img;
        img.load(names[i] + ".png");
        weaponPix[i] = QPixmap::fromImage(img.scaledToWidth(img.width() * imgScale));
        img.load(names[i] + "IK.png");
        for (int y = 0; y < img.height(); ++y) {
            QRgb *line = reinterpret_cast<QRgb *>(img.scanLine(y));
            for (int x = 0; x < img.width(); ++x) {
                if (line[x] == 0xFFFF00FF)
                    wIKs[i].anchorOffset = true;
                else if (line[x] == 0xFFFF0000)
                    wIKs[i].leftH = QPoint(x, y) * imgScale + QPoint(-weaponPix[i].width(), -weaponPix[i].height() / 2);
                else if (line[x] == 0xFF0000FF)
                    wIKs[i].rightH = QPoint(x, y) * imgScale + QPoint(-weaponPix[i].width(), -weaponPix[i].height() / 2);
                else if (line[x] == 0xFF00FF00)
                    wIKs[i].fireLoc = QPoint(x, y) * imgScale + QPoint(-weaponPix[i].width(), -weaponPix[i].height() / 2);
            }
        }
    }
    sameDir = true;
    adderleg = -0.01f;
    temp.load("bgPlats2Nu.png");
    for (int j = 0; j < temp.height() - 1; ++j) {
        QRgb *line = reinterpret_cast<QRgb *>(temp.scanLine(j));
        QRgb *line2 = reinterpret_cast<QRgb *>(temp.scanLine(j + 1));
        for (int i = 0; i < temp.width() - 1; ++i) {
            if ((line[i] == 0xFFFF0000 && line[i + 1] == 0xFFFF0000 && line2[i] == 0xFFFF0000) || (line[i] == 0xFF00FF00 && line[i + 1] == 0xFF00FF00 && line2[i] == 0xFF00FF00)) {
                int y = 0;
                while (temp.pixel(i, j + y) == 0xFFFF0000 || temp.pixel(i, j + y) == 0xFF00FF00 || temp.pixel(i, j + y) == 0xFF0000FF)
                    ++y;
                --y;
                int x = 0;
                while (temp.pixel(i + x, j + y) == 0xFFFF0000 || temp.pixel(i + x, j + y) == 0xFF00FF00 || temp.pixel(i + x, j + y) == 0xFF0000FF)
                    ++x;
                --x;
                bounds.push_back(QRect(i, j, x, y));
            }
        }
    }
    bagImg.load("bag.png");
    bagLoc = QPoint(-1000, -1000);
    temp.load("record.png");
    //testRecord = temp;
    record = vector <vector <unsigned char>> (temp.width(), vector <unsigned char> (temp.height(), 0));
    for (int j = 0; j < temp.height(); ++j) {
        QRgb *line = reinterpret_cast<QRgb *>(temp.scanLine(j));
        //QRgb *line2 = reinterpret_cast<QRgb *>(testRecord.scanLine(j));
        for (int i = 0; i < temp.width(); ++i) {
            if (line[i] != 0xFFFFFFFF)
                record[i][j] = 1;
            //line2[i] = record[i][j] ? 0xFF000000 : 0xFFFFFFFF;
        }
    }
    lead = Rope(bg.width() + bg.height() - 1, 2.0, 2, QColor(0xFF70451D), 0.02 + 0.04 * 2, 0.007 + 0.02 * 2);
    lead.maxSegments = bg.width() + bg.height();
    lead.numSegments = 30;
    lead.anchored = true;
    lead.isHair = false;
    lead.pushAnchor(lead.tracking.toPoint());

    setCursor(Qt::CrossCursor);
    keyDown = false;
    connect(&redraw, SIGNAL(timeout()), this, SLOT(nextFrame()));
    redraw.start(1000 / FPS);
}

MainWindow::~MainWindow() {
    redraw.stop();
    redraw.disconnect();
    cout << "average time: " << (avgFPS / static_cast<float>(frameCnt)) << "ms" << endl;
    cout << "min time: " << lowFPS << "ms" << endl;
    cout << "max time: " << highFPS << "ms" << endl;
    try {
        delete ui;
    }
    catch (...) {
        cout << "oops" << endl;
    }
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawPixmap(-px / 8, (mapBottom - py) / 80 + imgOffsets[3], bg3);
    painter.drawPixmap(-px / 4, (mapBottom - py) / 40 + imgOffsets[2], bg2);
    painter.drawPixmap(-px / 2, (mapBottom - py) / 20 + imgOffsets[1], bg1);
    painter.drawPixmap(-px, (mapBottom - py) / 10 + imgOffsets[0], bg);
    QPen pen = painter.pen();
    bool doBob = abs(xSpeed) > 2.0;
    if (doBob)
        painter.translate(0, 4 * (cos(pi * offset / (scale / 2)) + 1.0) / 2.0);
    if (LorR) {
        painter.scale(-1, 1);
        painter.translate(-px, py);
    }
    else
        painter.translate(px, py);
    pen.setWidth(limbs[2].width);
    pen.setColor(limbs[2].color);
    painter.setPen(pen);
    painter.drawLine(limbs[2].segments[0].line());
    QLine l2 = limbs[2].final.line();
    QLine l1 = l2;
    l2.setP1(l2.p1() * 0.7 + l2.p2() * 0.3);
    l1.setP2(l2.p1());
    pen.setColor(QColor(254, 197, 175));
    painter.setPen(pen);
    painter.drawLine(l1);
    pen.setColor(limbs[2].color);
    painter.setPen(pen);
    painter.drawLine(l2);
    painter.drawPixmap(-pW2, -pH, lA);

//    render rects
    if (showBounds) {
        QColor color;
        color.setRgbF((cos(pi * frameCnt * ifFPS) + 1.0) / 2.0, 0.0, 1.0 - (cos(pi * frameCnt * ifFPS) + 1.0) / 2.0);
        pen.setColor(color);
        painter.setPen(pen);
        painter.resetTransform();
        painter.translate(-px, (mapBottom - py) / 10);
        for (QRect rect : bounds)
            painter.drawRect(rect);
        QRect rect = QRect(bagLoc, bagLoc + QPoint(bagImg.width(), bagImg.height()));
        painter.drawRect(rect);
        painter.resetTransform();
        painter.drawRect(QRect(px - pW2, py - pH, pW, pH + 2));
        painter.resetTransform();
        painter.setPen(0xFF00FF00);
        QPoint offset = QPoint(-px, (mapBottom - py) / 10) + QPoint(0, imgOffsets[0]);
        if (testPts.size() > 1 && lead.anchored) {
            //painter.drawImage(0, 0, testRecord);
            //painter.setPen(0xFFFF0000);
            //painter.drawLine(testPts.front(), testPts.back());
            painter.drawLine(testPts.front() + offset, testPts.back() + offset);
        }
        else if (!lead.anchored)
            painter.drawLine(lead.segments[lead.numSegments - 2].posNow.toPoint() + offset, lead.anchors[lead.anchors.size() - 1] + offset - QPoint(0, imgOffsets[0]));
    }

    painter.resetTransform();
    if (doBob)
        painter.translate(0, 4 * (cos(pi * offset / (scale / 2)) + 1.0) / 2.0);
    if (LorR) {
        painter.scale(-1, 1);
        painter.translate(-px, py);
    }
    else
        painter.translate(px, py);
    pen.setWidth(limbs[0].width);
    pen.setColor(limbs[0].color);
    painter.setPen(pen);
    painter.drawLine(limbs[0].segments[0].line());
    painter.drawLine(QLine(eleg.toPoint(), limbs[0].segments[0].A.toPoint() + QPoint(2, 0)));
    painter.drawLine(QLine((eleg + limbs[0].anchor).toPoint() / 2, limbs[0].segments[0].A.toPoint() + QPoint(2, 0)));
    l2 = limbs[0].final.line();
    l1 = l2;
    l2.setP1(l2.p1() * 0.7 + l2.p2() * 0.3);
    l1.setP2(l2.p1());
    pen.setCapStyle(Qt::FlatCap);
    pen.setColor(QColor(177, 14, 22));
    painter.setPen(pen);
    painter.drawLine(l1);
    pen.setCapStyle(Qt::SquareCap);
    pen.setColor(limbs[0].color);
    painter.setPen(pen);
    painter.drawLine(l2);
    painter.drawLine(QLine(l2.p2() + QPoint(2, 0), l2.p1()));
    pen.setWidth(limbs[1].width);
    pen.setColor(limbs[1].color);
    painter.setPen(pen);
    painter.drawLine(limbs[1].segments[0].line());
    painter.drawLine(QLine(eleg.toPoint(), limbs[1].segments[0].A.toPoint() + QPoint(2, 0)));
    painter.drawLine(QLine((eleg + limbs[1].anchor).toPoint() / 2, limbs[1].segments[0].A.toPoint() + QPoint(2, 0)));
    l2 = limbs[1].final.line();
    l1 = l2;
    l2.setP1(l2.p1() * 0.7 + l2.p2() * 0.3);
    l1.setP2(l2.p1());
    pen.setCapStyle(Qt::FlatCap);
    pen.setColor(QColor(177, 14, 22));
    painter.setPen(pen);
    painter.drawLine(l1);
    pen.setCapStyle(Qt::SquareCap);
    pen.setColor(limbs[1].color);
    painter.setPen(pen);

    painter.drawLine(l2);
    painter.drawLine(QLine(l2.p2() + QPoint(2, 0), l2.p1()));
    painter.drawPixmap(0, 0, body);
    pen.setWidth(4.0);
    pen.setColor(0xFF111111);
    painter.setPen(pen);
    float off = (cos(pi * offset / scale) + 1.0) / 4.0;
    float angle = atan2(limbs[1].final.B.y() - limbs[1].final.A.y(), limbs[1].final.B.x() - limbs[1].final.A.x()) + pi - pi * 10 * (off + interpR / 2.0) / 180.0;
    float s = sin(angle);
    float c = cos(angle);

    QPoint p = limbs[1].final.A.toPoint();
    float xnew = -8.0 * s;
    float ynew = 8.0 * c;

    QLine l3 = QLine(p, QPoint(xnew, ynew) + p);
    painter.drawLine(l3);

    angle = atan2(limbs[0].final.B.y() - limbs[0].final.A.y(), limbs[0].final.B.x() - limbs[0].final.A.x()) + pi - pi * 10 * (0.5 - off + interpR / 2.0) / 180.0;
    s = sin(angle);
    c = cos(angle);

    p = limbs[0].final.A.toPoint();
    xnew = -8.0 * s;
    ynew = 8.0 * c;

    l3 = QLine(p, QPoint(xnew, ynew) + p);
    painter.drawLine(l3);



    if (mousePressed) {
        QPointF center = limbs[2].anchor + QPoint(px, py);
        if (wIKs[wIndex].anchorOffset)
            center += QPoint(0, limbs[2].first.length);
        float angle = atan2(pt.y() - center.y(), pt.x() - center.x()) + pi;
        float degs = angle * 180.0 / pi;
        if (degs > 90 && degs < 270)
            degs = -(degs - 180);
        painter.translate(limbs[2].anchor);
        if (wIKs[wIndex].anchorOffset)
            painter.translate(QPoint(0, limbs[2].first.length));
        painter.rotate(degs);
        painter.drawPixmap(QPoint(-weaponPix[wIndex].width(), -weaponPix[wIndex].height() / 2), weaponPix[wIndex]);
        painter.resetTransform();
        if (doBob)
            painter.translate(0, 4 * (cos(pi * offset / (scale / 2)) + 1.0) / 2.0);
        if (LorR) {
            painter.scale(-1, 1);
            painter.translate(-px, py);
        }
        else
            painter.translate(px, py);
    }

    pen.setWidth(limbs[3].width);
    pen.setColor(limbs[3].color);
    painter.setPen(pen);
    painter.drawLine(limbs[3].segments[0].line());
    l2 = limbs[3].final.line();
    l1 = l2;
    l2.setP1(l2.p1() * 0.7 + l2.p2() * 0.3);
    l1.setP2(l2.p1());
    pen.setColor(QColor(254, 197, 175));
    painter.setPen(pen);
    painter.drawLine(l1);
    pen.setColor(limbs[3].color);
    painter.setPen(pen);
    painter.drawLine(l2);
    painter.resetTransform();
    if (doBob)
        painter.translate(0, 4 * (cos(pi * offset / (scale / 2)) + 1.0) / 2.0);

    for (Rope &r : hairs) {
        pen.setColor(r.color);
        pen.setWidth(r.width);
        painter.setPen(pen);
        QPoint start = r.segments[0].posNow.toPoint();
        for (int i = 1; i < r.numSegments; ++i) {
            painter.drawLine(start, r.segments[i].posNow.toPoint());
            start = r.segments[i].posNow.toPoint();
        }
    }

    painter.resetTransform();
    painter.translate(-px, (mapBottom - py) / 10);
    if (!shotPts.empty()) {
        pen.setColor(QColor(127 + (rand() % 128), 0, 0));
        //pen.setColor(0xFFFF0000);
        painter.setPen(pen);
        for (int i = 0; i < shotPts.size() - 1; ++i) {
            pen.setWidth(1 + log(i + 1));
            painter.setPen(pen);
            painter.drawLine(shotPts[i], shotPts[i + 1]);
        }
    }
    else if (!bagArc.empty() && rightBtn) {
        pen.setColor(0xFFFFFF55);
        pen.setWidth(2);
        painter.setPen(pen);
        for (int i = 0; i < bagArc.size() - 1; ++i)
            painter.drawLine(bagArc[i], bagArc[i + 1]);
    }
    painter.drawPixmap(targetPt - QPoint(target.width() / 2, target.height() / 2), target);
    for (int i = 0; i < particles.size(); ++i)
        painter.drawPixmap(particles[i].first.x, particles[i].first.y, particleTrails[particles[i].second / 2]);
    for (int i = 0; i < otherParticles.size(); ++i)
        painter.drawPixmap(otherParticles[i].first.x, otherParticles[i].first.y, particleTrails[min(static_cast<int>(particleTrails.size() - 1), otherParticles[i].second)]);
    for (int i = 0; i < projectiles.size(); ++i) {
        if (projectiles[i].type == rocket) {
            painter.translate(projectiles[i].x, projectiles[i].y);
            painter.rotate(180.0 * projectiles[i].angle / pi);
            painter.drawPixmap(0, 0, projSprites[projectiles[i].type]);
            painter.resetTransform();
            painter.translate(-px, (mapBottom - py) / 10);
        }
        else
            painter.drawPixmap(projectiles[i].x, projectiles[i].y, projSprites[projectiles[i].type]);
    }
    painter.resetTransform();
    painter.translate(-px, (mapBottom - py) / 10);
    for (int i = 0; i < explosions.size(); ++i) {
        QPixmap qpm = expSeq[explosions[i].second];
        QPoint point = explosions[i].first - (QPoint(qpm.width(), qpm.height()) / 2);
        painter.drawPixmap(point, qpm);

    }
    painter.resetTransform();


    if (bagPlaced) {
        painter.translate(-px, (mapBottom - py) / 10);
        painter.drawPixmap(bagLoc, bagImg);
        pen.setColor(lead.color);
        pen.setWidth(5);
        painter.setPen(pen);
        QPointF handAdder = limbs[3].final.A;
        if (LorR)
            handAdder.setX(-1.0 * handAdder.x());
        for (int i = 1; i < lead.anchors.size(); ++i)
            painter.drawLine(lead.anchors[i - 1], lead.anchors[i]);
        QPoint start = lead.segments[0].posNow.toPoint();
        for (int i = 1; i < lead.numSegments; ++i) {
            painter.drawLine(start, lead.segments[i].posNow.toPoint());
            start = lead.segments[i].posNow.toPoint();
        }
        if (lead.anchored)
            painter.drawLine(start, QPoint(2 * px, py - (mapBottom - py) / 10) + handAdder);
        painter.setPen(0xFF00FF00);

        if (showBounds && testPts.size() > 1 && lead.anchored) {
            //painter.drawImage(0, 0, testRecord);
            //painter.setPen(0xFFFF0000);
            //painter.drawLine(testPts.front(), testPts.back());
            painter.translate(0, imgOffsets[0]);
            painter.drawLine(testPts.front(), testPts.back());
            painter.translate(0, -imgOffsets[0]);
            for (int i = 0; i < lead.anchors.size(); ++i) {
                srand(i + 1);
                painter.setPen(QColor(rand() % 255, rand() % 255, rand() % 255));
                painter.drawEllipse(lead.anchors[i], 5 * (i +1), 5 * (i + 1));
            }
        }
    }
    painter.resetTransform();
}

void MainWindow::nextFrame() {
    logTime();
    targetPt = inbetweening * lastTar + (1.0 - inbetweening) * nextTar;
    inbetweening -= 0.0025f;
    if (inbetweening <= 0.0) {
        lastTar = nextTar;
        nextTar = QPoint(rand() % WIDTH, rand() % HEIGHT);
        inbetweening = 1.0;
    }
    float dir = lastKey == Qt::Key_Left ? -1.0 : 1.0;
    if (keyDown)
        xSpeed += dir * xAcc;
    else {
        xSpeed *= onGround ? groundFrict : airFrict;
        if (abs(xSpeed) * ifFPS <= 1.0)
            xSpeed = 0.0;
    }
    float check = dir * xSpeed;
    if (check > xImpulse)
        xSpeed *= groundFrict;
    ySpeed += gravity * ifFPS;
    if (abs(ySpeed * ifFPS) <= gravity * ifFPS && onGround)
        ySpeed = 0.0;
    lastX = px;
    lastY = py;
    QPointF change(px, py);
    if (lead.anchored)
        lastPt = change;
    px += xSpeed * ifFPS;
    py += ySpeed * ifFPS;
    if (climbing) {
        if (whichSeg > 0) {
            --whichSeg;
            QPoint offset = QPoint(-px, (mapBottom - py) / 10);
            px = lead.segments[whichSeg].posNow.x() + offset.x();
            py = lead.segments[whichSeg].posNow.y() + offset.y();
            xSpeed = 0;
            ySpeed = 0;
            onGround = false;
            LorR = !LorR;
        }
        else {
            climbing = false;
            interpC = 0.0;
        }
    }
    onGround = abs(py - mapBottom) < 2.0;
    QRect pRect = QRect(2.0 * px - pW2, py - pH - (mapBottom - py) / 10, pW, pH + 2);
    for (QRect rect : bounds)
        if (rect.intersects(pRect)) {
            if (pRect.bottom() >= rect.top() && pRect.top() <= rect.top() && pRect.bottom() + 1 < rect.bottom()) {
                ySpeed = (-ySpeed * yBounceCoef) - 1;
                py = rect.top() + (mapBottom - py) / 10 + ySpeed * ifFPS;
                hasDJ = true;
                onGround = abs(py - rect.top() - (mapBottom - py) / 10) < 2.0;
            }
            else if (pRect.top() < rect.bottom() && pRect.bottom() - 1 > rect.bottom()) {
                py = rect.bottom() + pH + (mapBottom - py) / 10 + 1;
                ySpeed = 0;
            }
            else if (pRect.center().x() < rect.center().x()) {
                px = rect.left() / 2 - pW2 / 2 - 1;
                xSpeed = -xSpeed;
                px += 2.0 * xSpeed * ifFPS;
                xSpeed *= xBounceCoef;
            }
            else if (pRect.center().x() > rect.center().x()) {
                px = rect.right() / 2 + pW2 / 2 + 1;
                xSpeed = -xSpeed;
                px += 2.0 * xSpeed * ifFPS;
                xSpeed *= xBounceCoef;
            }
            break;
        }
    if (!onGround && !reversed) {
        reverse(bounds.begin(), bounds.end());
        reversed = true;
    }
    else if (onGround && reversed) {
        reverse(bounds.begin(), bounds.end());
        reversed = false;
    }


    if (py >= mapBottom) {
        py = mapBottom - 1;
        hasDJ = true;
        ySpeed = -ySpeed * yBounceCoef;
    }
    else if (py < 0 && ySpeed < 0)
        ySpeed *= groundFrict;
    if (px >= WIDTH || px < 0) {
        xSpeed = -xSpeed;
        px += 2.0 * xSpeed * ifFPS;
        xSpeed *= xBounceCoef;
    }
    interpR = min(abs(xSpeed), xImpulse) / static_cast<float>(xImpulse);
    if (throwingStatus == 1 && interpC < 1.0) {
        interpC += 0.05;
        if (interpC >= 1.0) {
            throwingStatus = 2;
            interpC = 1.0;
        }
    }
    else if (throwingStatus == 2 && interpC > 0.0) {
        interpC -= 0.05;
        if (interpC <= 0.0) {
            throwingStatus = 0;
            interpC = 0.0;
        }
    }


    change -= QPointF(px, py);
    for (int i = 0; i < hairs.size(); ++i)
        hairs[i].tracking -= change;
    pt = mapFromGlobal(QCursor::pos());
    sameDir = true;
    if (mousePressed) {
        if (pt.x() < px) {
            if (LorR == true) {
                QPointF center = QPointF(px - 1, 0.0);
                for (int i = 0; i < hairs.size(); ++i) {
                    center.setY(hairs[i].tracking.y());
                    change = center - hairs[i].tracking;
                    hairs[i].tracking = center + change;
                }
            }
            LorR = false;
        }
        else if (pt.x() > px) {
            if (LorR == false) {
                QPointF center = QPointF(px - 1, 0.0);
                for (int i = 0; i < hairs.size(); ++i) {
                    center.setY(hairs[i].tracking.y());
                    change = center - hairs[i].tracking;
                    hairs[i].tracking = center + change;
                }
            }
            LorR = true;
        }
    }
    else if (change.x() != 0.0) {
        if (LorR != change.x() < 0.0) {
            QPointF center = QPointF(px - 1, 0.0);
            for (int i = 0; i < hairs.size(); ++i) {
                center.setY(hairs[i].tracking.y());
                change = center - hairs[i].tracking;
                hairs[i].tracking = center + change;
            }
        }
        LorR = change.x() < 0.0;
        if (onGround)
            for (int i = 0; i < hairs.size(); ++i) {
                for (int j = 1; j < hairs[i].numSegments; ++j) {
                    hairs[i].segments[j].posNow.setX((hairs[i].segments[j - 1].posNow.x() * 0.3 + hairs[i].segments[j].posNow.x() * 0.7));
                }
            }
    }
    if (LorR)
        sameDir = xSpeed >= 0;
    else
        sameDir = xSpeed <= 0;


    if (throwingStatus == 0 && interpC > 0.0 && interpC < 1.0) {
        interpC += adderleg;
        if (interpC < 0.0)
            interpC = 0.0;
        else if (interpC > 1.0)
            interpC = 1.0;
    }
    float offsetAdder = onGround || abs(py - mapBottom) <= 20 ? 0.16 : 0.02;
//    if (abs(xSpeed) < 2)
//        offset = 10 * offset / 12;
//    else
        offset -= (offsetAdder + interpR * offsetAdder);
    if (offset <= -scale) {
        offset = scale;
        whichLeg = !whichLeg;
    }
    else if (abs(offset) < (offsetAdder + interpR * offsetAdder) / 2.0)
        whichLeg = !whichLeg;
    dleg = QPointF(0.0, (2.25 + interpR * 0.5) * limbs[0].segments[0].length);
    dleg = sameDir ? limbs[0].anchor - dleg : limbs[0].anchor + dleg;
    float interp;
    QPointF bOff;
    if (dleg.y() < (rtleg.y() + ltleg.y()) / 2.0)
        interp = (cos(pi * offset / scale) + 1.0) / 2.0;
    else {
        interp = -abs(offset/ scale) + 1.0;
        QPointF tar = (ltleg - rtleg) / 2.0;
        bOff = QPointF(-abs(tar.x()) / (2.0 + 1.5 * interpR), abs(tar.y() * (1.0 + interpR / 2.0)));
    }
    if (abs(xSpeed) <= 2.0)
        interp = 0.5;
    float interpI = 1.0 - interp;
    if (!flipper) {
        ltleg = (1.0 - interpR) * (asleg * interp + aeleg * interpI) + interpR * (bsleg * interp + beleg * interpI) + bOff;
        rtleg = (1.0 - interpR) * (asleg * interpI + aeleg * interp) + interpR * (bsleg * interpI + beleg * interp) + bOff;
    }
    else {
        rtleg = (1.0 - interpR) * (asleg * interp + aeleg * interpI) + interpR * (bsleg * interp + beleg * interpI) + bOff;
        ltleg = (1.0 - interpR) * (asleg * interpI + aeleg * interp) + interpR * (bsleg * interpI + beleg * interp) + bOff;
    }
    if (dleg.y() > (rtleg.y() + ltleg.y()) / 2.0) {
        rtleg.setY((ltleg.y() + rtleg.y()) / 2.0);
        ltleg.setY(rtleg.y());
    }
    if (abs(xSpeed) <= 2.0) {
        rtleg += QPointF(0.0, -(dleg.y() - rtleg.y()) * (-abs(interp - 0.5) + 0.5));
        ltleg += QPointF(0.0, -(dleg.y() - ltleg.y()) * (-abs(interp - 0.5) + 0.5));
    }
    if (whichLeg)
        rtleg += QPointF(0.0, -(dleg.y() - rtleg.y()) * (-abs(interp - 0.5) + 0.5));
    else
        ltleg += QPointF(0.0, -(dleg.y() - ltleg.y()) * (-abs(interp - 0.5) + 0.5));

    interp = max(interp, interpI);
    interpI = 1.0 - interp;
    rtleg = rtleg * (1.0 - interpC) + interpC * (csleg * interp + celeg * interpI);
    ltleg = ltleg * (1.0 - interpC) + interpC * (csleg * interpI + celeg * interp);

    limbs[0].solve(ltleg);
    float angle1 = -atan2(limbs[0].final.A.y() - limbs[0].anchor.y(), limbs[0].final.A.x() - limbs[0].anchor.x());
    float angle2 = -atan2(limbs[0].final.B.y() - limbs[0].anchor.y(), limbs[0].final.B.x() - limbs[0].anchor.x());
    //limbs[0].final.angle = angle1;
    bool check2 = angle2 < -pi / 2 && angle1 > pi / 2;
    if (angle2 > angle1 || check2) {
        QPointF mid = (limbs[0].final.A + limbs[0].anchor) / 2.0;
        limbs[0].segments[0].A -= mid;
        limbs[0].segments[0].A *= -1.0;
        limbs[0].segments[0].A += mid;
    }

    limbs[1].solve(rtleg);
    angle1 = -atan2(limbs[1].final.A.y() - limbs[1].anchor.y(), limbs[1].final.A.x() - limbs[1].anchor.x());
    angle2 = -atan2(limbs[1].final.B.y() - limbs[1].anchor.y(), limbs[1].final.B.x() - limbs[1].anchor.x());
    //limbs[1].final.angle = angle1;
    check2 = angle2 < -pi / 2 && angle1 > pi / 2;
    if (angle2 > angle1 || check2) {
        QPointF mid = (limbs[1].final.A + limbs[1].anchor) / 2.0;
        limbs[1].segments[0].A -= mid;
        limbs[1].segments[0].A *= -1.0;
        limbs[1].segments[0].A += mid;
    }


    interpR = 1.0 - interpR;
    float interpB = (cos(pi * offset / scale) + 1.0) / 2.0;
    if (abs(xSpeed) <= 2.0)
        interpB = 0.5;
    float interpA = 1.0 - interpB;
    a = as * interpR + ae * (1.0 - interpR);
    b = bs * interpR + be * (1.0 - interpR);
    QPointF point = a * interpA + b * interpB;
    point = point * (1.0 - interpC) + interpC * (cs * interpA + ce * interpB);
    QPointF center;
    float angle, s, c, angleShot;
    if (mousePressed) {
        center = limbs[2].anchor;
        if (wIKs[wIndex].anchorOffset)
            center += QPoint(0, limbs[2].first.length);
        angleShot = angle = atan2(pt.y() - center.y() - py - 10, pt.x() - center.x() - px) + pi;
        if (angle > pi * 0.5 && angle < pi * 1.5) {
            angle = -(angle - pi);
            angleShot -= pi / 22.5f;
        }
        s = sin(angle);
        c = cos(angle);

        point = wIKs[wIndex].fireLoc;
        float C = cos(angleShot);
        float S = sin(angleShot);
        float xnew = point.x() * C - point.y() * S;
        float ynew = point.x() * S + point.y() * C;
        fixedFLoc = wIKs[wIndex].fixedFireLoc = QPoint(xnew, ynew) + center.toPoint();
        // rotate point
        point = wIKs[wIndex].rightH;
        xnew = point.x() * c - point.y() * s;
        ynew = point.x() * s + point.y() * c;

        // translate point back:
        point = QPoint(xnew, ynew) + center;
    }
    limbs[2].solve(point);
    //first.b   and    seg[0].a
    angle1 = -atan2(limbs[2].final.A.y() - limbs[2].anchor.y(), limbs[2].final.A.x() - limbs[2].anchor.x());
    angle2 = -atan2(limbs[2].final.B.y() - limbs[2].anchor.y(), limbs[2].final.B.x() - limbs[2].anchor.x());
    check2 = angle2 < -pi / 2 && angle1 > pi / 2;
    if (angle2 < angle1 && !check2) {
        QPointF mid = (limbs[2].final.A + limbs[2].anchor) / 2.0;
        limbs[2].segments[0].A -= mid;
        limbs[2].segments[0].A *= -1.0;
        limbs[2].segments[0].A += mid;
    }
    point = a * interpB + b * interpA;
    point = point * (1.0 - interpC) + interpC * (cs * interpB + ce * interpA);
    if (mousePressed) {
        point = wIKs[wIndex].leftH;
        // rotate point
        float xnew = point.x() * c - point.y() * s;
        float ynew = point.x() * s + point.y() * c;

        // translate point back:
        point = QPoint(xnew, ynew) + center;
    }
    limbs[3].solve(point);
    angle1 = -atan2(limbs[3].final.A.y() - limbs[3].anchor.y(), limbs[3].final.A.x() - limbs[3].anchor.x());
    angle2 = -atan2(limbs[3].final.B.y() - limbs[3].anchor.y(), limbs[3].final.B.x() - limbs[3].anchor.x());
    check2 = angle2 < -pi / 2 && angle1 > pi / 2;
    if (angle2 < angle1 && !check2) {
        QPointF mid = (limbs[3].final.A + limbs[3].anchor) / 2.0;
        limbs[3].segments[0].A -= mid;
        limbs[3].segments[0].A *= -1.0;
        limbs[3].segments[0].A += mid;
    }
    interpR = 1.0 - interpR;

    shotPts.clear();
    if (!rightBtn && !bagArc.empty()) {
        QPoint bagLocOld = bagLoc;
        bagLoc = bagArc.back().toPoint();
        bagArc.pop_back();
        lead.tracking = bagLoc + QPoint(bagImg.width() / 2, bagImg.height() / 5);
        lead.anchors[0] = lead.tracking.toPoint();
        if (lead.anchors.size() > 2) {
            lead.anchors[1] = lead.anchors[0];
            float mn = static_cast<float>(lead.tracking.y() - bagLocOld.y());
            float md = static_cast<float>(lead.tracking.x() - bagLocOld.x());
            float dist = sqrt(mn * mn + md * md);
            float adder = 1.0 / dist;
            for (float r = 0.0; r <= dist; r += adder) {
                float div = min(r / dist, 1.0f);
                QPointF inbet = div * bagLocOld + (1.0 - div) * lead.tracking;
                pair <bool, QPoint> ret = raycast(inbet.toPoint(), lead.anchors[2]);
                if (ret.first == false) {
                    lead.pushFront(ret.second);
                }
            }
            lead.tracking = lead.anchors.back();
        }
    }
    if (bagPlaced && lead.anchors[1] != lead.anchors[0])
        lead.anchors[1] = lead.anchors[0];

    QThread *solver2 = nullptr;
    if (bagPlaced) {
        if (lead.anchored) {
            QPointF handAdder = limbs[3].final.A;
            if (LorR)
                handAdder.setX(-1.0 * handAdder.x());
            lead.anchor = QPoint(2 * px, py - (mapBottom - py) / 10) + handAdder;
            lastPt = QPointF(2.0 * lastPt.x(), lastPt.y() - (mapBottom - lastPt.y()) / 10) + handAdder;
            if (lead.anchors.size() >= 1) {
                float mn = static_cast<float>(lead.anchor.y() - lastPt.y());
                float md = static_cast<float>((lead.anchor.x() - lastPt.x()));
                float dist = sqrt(mn * mn + md * md);
                float angle = atan2(mn, md);
                float s = sin(angle);
                float c = cos(angle);
                int currAnchor = lead.anchors.size() - 1;
                float adder = 1.0 / dist;
                for (float r = 0.0; r <= dist; r += adder) {
                    float xnew = r * c;
                    float ynew = r * s;
                    QPointF point(xnew, ynew);
                    point += lastPt;
                    pair <bool, QPoint> ret = raycast(point.toPoint(), lead.anchors[currAnchor]);
                    if (ret.first == false) {
                        lead.pushAnchor(ret.second);
                        ++currAnchor;
                    }
                    else if (currAnchor > 0) {
                        ret = raycast(point.toPoint(), lead.anchors[currAnchor - 1]);
                        pair <bool, QPoint> ret1 = raycast(point.toPoint(), (19 * lead.anchors[currAnchor] + lead.anchors[currAnchor - 1]) / 20);
                        if (ret.first == true && ret1.first == true) {
                            lead.popAnchor();
                            --currAnchor;
                            r -= adder;
                        }
                    }
                }
                if (showBounds)
                    raycast(point.toPoint(), lead.anchors[currAnchor]);
                if (lead.anchors.size() > 2)
                    raycast(lead.anchors[1], lead.anchors[2]);
            }

            float mn = static_cast<float>(lead.tracking.y() - lead.anchor.y());
            float md = static_cast<float>((lead.tracking.x() - lead.anchor.x()));
            float dist = sqrt(mn * mn + md * md) / (log10(lead.numSegments + 10));
            if (lead.numSegments < dist && dist < lead.maxSegments) {
                lead.segments[lead.numSegments].posNow = lead.segments[lead.numSegments - 1].posNow;
                ++lead.numSegments;
            }
            else if (lead.numSegments > dist && lead.numSegments > 1)
                --lead.numSegments;

            bool found = false;
            for (int i = 0; i < lead.numSegments; ++i) {
                QPoint pt = lead.segments[i].posNow.toPoint();
                for (QRect rect : bounds)
                    if (rect.contains(pt)) {
                        found = true;
                        break;
                    }
                if (found || pt.y() >= mapBottom) {
                    --lead.numSegments;
                    break;
                }
            }
        }
        else {
            lead.anchor = lead.segments[lead.numSegments - 1].posNow;
            if (false && lead.anchors.size() >= 1 && (lead.anchor - lastPt).toPoint() != QPoint(0, 0)) {
                QPoint pt = lead.segments[lead.numSegments - 1].posNow.toPoint();
                float mn = static_cast<float>(lead.anchor.y() - lastPt.y());
                float md = static_cast<float>((lead.anchor.x() - lastPt.x()));
                float dist = sqrt(mn * mn + md * md);
                float angle = atan2(mn, md);
                float s = sin(angle);
                float c = cos(angle);
                int currAnchor = lead.anchors.size() - 1;
                float adder = 1.0 / dist;
                for (float r = 0.0; r <= dist; r += adder) {
                    float xnew = r * c;
                    float ynew = r * s;
                    QPointF point(xnew, ynew);
                    point += lastPt;
                    pair <bool, QPoint> ret = raycast(point.toPoint(), lead.anchors[currAnchor]);
                    if (ret.first == false) {
                        lead.pushAnchor(ret.second);
                        ++currAnchor;
                    }
                    else if (currAnchor > 0) {
                        ret = raycast(point.toPoint(), lead.anchors[currAnchor - 1]);
                        pair <bool, QPoint> ret1 = raycast((lead.anchors[currAnchor] + pt) / 2, (3 * lead.anchors[currAnchor] + lead.anchors[currAnchor - 1]) / 4);
                        if (ret.first == true && ret1.first == true) {
                            lead.popAnchor();
                            --currAnchor;
                            r -= adder;
                        }
                        if (showBounds)
                            raycast(point.toPoint(), lead.anchors[currAnchor]);
                    }
                }
            }
            lastPt = lead.anchor;
            QRect pRect = QRect(px, py - pH, pW, pH + 2);
            QPointF offset = QPoint(-px, (mapBottom - py) / 10);
            for (int i = 1; i < lead.numSegments - 1; ++i)
                if (pRect.contains((lead.segments[i].posNow + offset).toPoint()))
                    lead.segments[i].posOld -= QPointF(xSpeed * ifFPS * 0.25, ySpeed * ifFPS * 0.25);
        }
        solver2 = QThread::create(ropeSolver, &lead);
        solver2->start();
    }




    int start = 0;//static_cast<int>(hairs.size() / 2);
    QThread *solver = QThread::create(hairSolver, &hairs, start, hairs.size());
    solver->start();

    for (int i = 0; i < start; ++i)
        hairs[i].solve();
    //long long time2 = getTime();
    for (int i = 0; i < shotParticle; ++i)
        if (timers[i] > 0)
            --timers[i];

    if ((mousePressed && timers[weapon] == 0) || rightBtn) {
        QPointF fLoc = fixedFLoc + QPointF(px, py) + QPoint(px, -(mapBottom - py) / 10);
        if (weapon == flamethrower || weapon == shotgun) {
            QPoint end = (pt + QPoint(px, py) + fixedFLoc.toPoint()) / 2.0;
            int projs = (fastrand() % 4) * 2 + 5 - weapon / 2;
            int spread = fastrand() % 30 + 40;
            int val = abs(end.x() - fixedFLoc.x()) + abs(end.y() - fixedFLoc.y());
            if (spread > val)
                spread = val - 1;
            int hSpread = spread / 2;
            for (int i = 0; i < projs; ++i) {
                QPoint res = end + QPoint((fastrand() % spread) - hSpread, (fastrand() % spread) - hSpread) + QPoint(px, -(mapBottom - py) / 10);
                projectiles.push_back(projectile(fLoc.x(), fLoc.y(), res, weapon));
            }
        }
        else if (weapon == laser) {
            projectile RopeShot = projectile(fLoc.x(), fLoc.y(), pt + QPoint(px, -(mapBottom - py) / 10), weapon);
            bool breaker = true;
            while (breaker) {
                QPointF shotpt = QPointF(RopeShot.x, RopeShot.y);
                shotPts.push_back(shotpt);
                for (QRect rect : bounds)
                    if (rect.contains(shotpt.toPoint())) {
                        breaker = false;
                        shotPts.pop_back();
                        break;
                    }
                if (shotpt.x() < -4000 || shotpt.y() < -2000 || shotpt.x() > 4000 || shotpt.y() > mapBottom) {
                    breaker = false;
                    shotPts.pop_back();
                }
                RopeShot.advance();
            }
            for (int i = 0; i < 5; ++i)
                particles.push_back(Particle(projectile(shotPts.back().x(), shotPts.back().y(), QPoint(fastrand() % WIDTH, fastrand() % mapBottom), expParticle), 255));
        }
        else if (rightBtn) {
            fLoc = QPointF(px, py) + QPoint(px, -(mapBottom - py) / 10);
            projectile RopeShot = projectile(fLoc.x(), fLoc.y() - (4 * pH2 / 3), pt + QPoint(px, -(mapBottom - py) / 10), ropeShot);
            bool breaker = true;
            bagArc.clear();
            while (breaker) {
                QPointF shotpt = QPointF(RopeShot.x, RopeShot.y);
                bagArc.push_back(shotpt);
                for (QRect rect : bounds)
                    if (rect.contains(shotpt.toPoint())) {
                        breaker = false;
                        bagArc.pop_back();
                        break;
                    }
                if (shotpt.x() < -4000 || shotpt.y() < -2000 || shotpt.x() > 4000 || shotpt.y() > mapBottom) {
                    breaker = false;
                    bagArc.pop_back();
                }
                RopeShot.advance();
            }
        }
        else
            projectiles.push_back(projectile(fLoc.x(), fLoc.y(), pt + QPoint(px, -(mapBottom - py) / 10), weapon));
        timers[weapon] = cooldown[weapon];
        if (cooldown[weapon] > 2 && xSpeed == 0.0)
            xSpeed += 2.1 * xImpulse / (numWeapons - weapon) * (LorR ? -1 : 1);
    }

    bool frameSkip = frameCnt % 2;
    for (int i = 0; i < projectiles.size(); ++i) {
        projectiles[i].advance();
        int X = projectiles[i].x;
        int Y = projectiles[i].y;
        if (projectiles[i].type == flamethrower)
            particles.push_back(Particle(projectile(X, Y, QPoint(fastrand() % WIDTH, fastrand() % mapBottom), shotParticle), 255));
        else if (projectiles[i].type == launcher && frameSkip)
            otherParticles.push_back(Particle(projectile(X + projSprites[launcher].width() * 0.75f, Y + projSprites[launcher].height() * 0.2, QPoint(fastrand() % WIDTH, fastrand() % mapBottom), shotParticle), 255));
        else if (projectiles[i].type == rocket && frameSkip) {
            float angle = projectiles[i].angle;
            float s = sin(angle);
            float c = cos(angle);
            float x = projSprites[rocket].width();
            float y = projSprites[rocket].height() / 2;
            float x1 = x * c - y * s;
            float y1 = x * s + y * c;
            float x2 = (x + (fastrand() % 5) * projectiles[i].xInc) * c - (y + (fastrand() % 5) * projectiles[i].yInc) * s;
            float y2 = (x + (fastrand() % 5) * projectiles[i].xInc) * s + (y + (fastrand() % 5) * projectiles[i].yInc) * c;
            otherParticles.push_back(Particle(projectile(X + x1, Y + y1, QPoint(X, Y) + QPoint(x2, y2), shotParticle), 255));
        }
        //int offset = projSprites[projectiles[i].type].width() / 2;
        bool flag = false;
        for (QRect rect : bounds)
            if (rect.contains(X, Y)) {
                flag = true;
                break;
            }
        if (projectiles[i].type == rocket || projectiles[i].type == launcher) {
            if (!shotPts.empty()) {
                QPointF dPt = QPointF(projectiles[i].x, projectiles[i].y);
                for (QPointF qpt : shotPts) {
                    QPointF avg = dPt - qpt;
                    if (abs(avg.x()) + abs(avg.y()) < projSprites[projectiles[i].type].width()) {
                        flag = true;
                        break;
                    }
                }
            }
        }
        if (X < px - bg.width() || Y < py - 2 * bg.height() || X >= px + bg.width() || Y >= mapBottom + (mapBottom - py - 5) / 10 || flag) {
            if (projectiles[i].type == flamethrower) {
                particles.push_back(Particle(projectile(X, Y, QPoint(fastrand() % WIDTH, fastrand() % mapBottom), expParticle), 255));
            }
            else if (projectiles[i].type == automatic) {
                if (fastrand() % 3)
                    particles.push_back(Particle(projectile(X, Y, QPoint(fastrand() % WIDTH, fastrand() % mapBottom), shotParticle), 255));
                else
                    particles.push_back(Particle(projectile(X, Y, QPoint(fastrand() % WIDTH, fastrand() % mapBottom), expParticle), 255));
            }
            else if (projectiles[i].type == shotgun) {
                if (fastrand() % 3)
                    particles.push_back(Particle(projectile(X, Y, QPoint(fastrand() % WIDTH, fastrand() % mapBottom), fastrand() % 2 ? shotParticle : expParticle), 255));
                particles.push_back(Particle(projectile(X, Y, QPoint(fastrand() % WIDTH, fastrand() % mapBottom), expParticle), 255));
                particles.push_back(Particle(projectile(X, Y, QPoint(fastrand() % WIDTH, fastrand() % mapBottom), shotParticle), 255));
            }
            else if (projectiles[i].type == launcher || projectiles[i].type == rocket) {
                explosions.push_back(Explosion(QPoint(X, Y), 0));
                for (int j = 0; j < 40; ++j)
                    particles.push_back(Particle(projectile(X, Y, QPoint(fastrand() % WIDTH, fastrand() % mapBottom), expParticle), 255));
            }
            else if (projectiles[i].type == bouncer) {
                QPoint where = QPoint(projectiles[i].x, projectiles[i].y);
                float angle = -180.0f;
                float radius = 100.0;
                float deg2rad = pi / 180.0f;
                while (angle <= 175.0f) {
                    QPoint to = where + QPoint(-radius * sin(angle * deg2rad), radius * cos(angle * deg2rad));
                    projectiles.push_back(projectile(where.x(), where.y(), to, shotgun));
                    angle += 10.0f;
                }
                int n = fastrand() % 30;
                for (int j = 0; j < n; ++j)
                    particles.push_back(Particle(projectile(X, Y, QPoint(fastrand() % WIDTH, fastrand() % mapBottom), expParticle), 255));
                for (int j = 0; j < 30 - n; ++j)
                    particles.push_back(Particle(projectile(X, Y, QPoint(fastrand() % WIDTH, fastrand() % mapBottom), shotParticle), 255));
            }
            else if (projectiles[i].type == silver) {
                projectiles[i].x += projectiles[i].xInc;
                projectiles[i].y += projectiles[i].yInc;
                bool sType = rand() % 2;
                QPoint where = QPoint(projectiles[i].x, projectiles[i].y);
                float angle = -180.0f;
                float radius = 100.0;
                float deg2rad = pi / 180.0f;
                while (angle <= 179.0f) {
                    QPoint to = where + QPoint(-radius * sin(angle * deg2rad), radius * cos(angle * deg2rad));
                    if (sType) {
                        projectiles.push_back(projectile(where.x(), where.y(), to, bouncer));
                        projectiles.push_back(projectile(where.x(), where.y(), to, rocket));
                        projectiles.back().advance();
                    }
                    else {
                        projectiles.push_back(projectile(where.x(), where.y(), to, launcher));
                        projectiles.back().advance();
                    }
                    angle += 2.0f;
                }
                int n = fastrand() % 30;
                for (int j = 0; j < n; ++j)
                    particles.push_back(Particle(projectile(X, Y, QPoint(fastrand() % WIDTH, fastrand() % mapBottom), expParticle), 255));
                for (int j = 0; j < 30 - n; ++j)
                    particles.push_back(Particle(projectile(X, Y, QPoint(fastrand() % WIDTH, fastrand() % mapBottom), shotParticle), 255));
            }
            projectiles.erase(projectiles.begin() + i);
            --i;
        }
    }
    if (!(frameCnt % 3)) {
        for (int i = 0; i < explosions.size(); ++i) {
            if (explosions[i].second % 4 == 0)
                expAffect(explosions[i].first - QPoint(px, -(mapBottom - py) / 10));
            ++explosions[i].second;
            if (explosions[i].second == expSeq.size()) {
                explosions.erase(explosions.begin() + i);
                --i;
            }
        }
    }
    for (int i = 0; i < particles.size(); ++i) {
        particles[i].second -= 2;
        particles[i].first.advance();
        if (particles[i].second <= 0 || particles[i].first.y >= mapBottom + (mapBottom - py) / 10) {
            particles.erase(particles.begin() + i);
            --i;
        }
    }
    for (int i = 0; i < otherParticles.size(); ++i) {
        otherParticles[i].second -= 16;
        otherParticles[i].first.advance();
        if (otherParticles[i].second <= 0 || otherParticles[i].first.y >= mapBottom + (mapBottom - py) / 10) {
            otherParticles.erase(otherParticles.begin() + i);
            --i;
        }
    }
    solver->wait();
    if (solver2 != nullptr)
        solver2->wait();



    repaint();
    getTimeSilent();
    avgFPS += t;
    lowFPS = min(lowFPS, static_cast<int>(t));
    highFPS = max(highFPS, static_cast<int>(t));
    ++frameCnt;
}

void MainWindow::expAffect(QPoint where) {
    QRect pSpace = QRect(px, py - pH, pW, pH);
    QPoint expOffset = QPoint(expDistance, expDistance);
    QRect expSpace = QRect(where - expOffset, where + expOffset);
    if (pSpace.intersects(expSpace)) {
        float angle = atan2((py - pH2) - where.y(), px - where.x()) + pi / 2.0;
        xSpeed += expForce * sin(angle);
        ySpeed += 2 * expForce * cos(angle);
    }
}

pair <bool, QPoint>  MainWindow::raycast(QPoint from, QPoint to) {
    QPoint offset = QPoint(0, -imgOffsets[0]);
    from += offset;
    to += offset;
    pair <bool, QPoint> ret (true, from);
    float mn = static_cast<float>(to.y() - from.y());
    float md = static_cast<float>((to.x() - from.x()));
    float dist = sqrt(mn * mn + md * md);
    float angle = atan2(mn, md);
    float s = sin(angle);
    float c = cos(angle);
    testPts.clear();
    if (dist <= 3.0) {
        float adder = 1.0 / dist;
        for (float r = 0.0; r < dist; r += adder) {
            float xnew = r * c;
            float ynew = r * s;
            QPoint point(xnew, ynew);
            point += from;
            testPts.push_back(point);
            if (point.x() >= 0 && point.y() >= 0 && point.x() < record.size() && point.y() < record[0].size() && record[point.x()][point.y()]) {
                ret.first = false;
                break;
            }
            ret.second = point;
        }
    }
    else {
        int dist1 = static_cast<int>(dist);
        float xnew = from.x(), ynew = from.y();
        for (int r = 1; r < dist1; ++r) {
            xnew += c;
            ynew += s;
            QPoint point (xnew, ynew);
            testPts.push_back(point);
            if (point.x() >= 0 && point.y() >= 0 && point.x() < record.size() && point.y() < record[0].size() && record[point.x()][point.y()]) {
                ret.first = false;
                break;
            }
            ret.second = point;
        }
    }
    ret.second -= offset;
    return ret;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_E)
        flipper = !flipper;
    switch (event->key()) {
        case Qt::Key_A:
        case Qt::Key_Left:
            lastKey = Qt::Key_Left;
            keyDown = true;
            break;
        case Qt::Key_D:
        case Qt::Key_Right:
            lastKey = Qt::Key_Right;
            keyDown = true;
            break;
    }
    switch (event->key()) {
        case Qt::Key_W:
        case Qt::Key_Up:
            if (hasDJ && !onGround) {
                hasDJ = false;
                interpC = 0.95;
            }
            else if (!onGround)
                break;
            ySpeed = yImpulse;
            onGround = false;
            py -= 1;
            for (int i = 0; i < hairs.size(); ++i)
                hairs[i].tracking += QPointF(0.0, -1.0);
            break;
        case Qt::Key_S:
        case Qt::Key_Down:
            if (!onGround || mousePressed)
                break;
            QRect pRect = QRect(px - pW2, py - pH, 2 * pW, pH + 2);
            QPoint offset = QPoint(-px, (mapBottom - py) / 10);
            QRect rect = QRect(bagLoc + offset, bagLoc + QPoint(bagImg.width(), bagImg.height()) + offset);
            if (bagPlaced) {
                if (pRect.intersects(rect)) {
                    bagPlaced = false;
                    bagLoc = QPoint(-1000, -1000);
                    while (lead.anchors.size() > 1)
                        lead.popAnchor();
                }
                else if (!lead.anchored) {
                    for (RopeSegment &rseg : lead.segments)
                        if (pRect.contains(rseg.posNow.toPoint() + offset)) {
                            lead.anchored = true;
                            break;
                        }
                }
                else if (lead.anchored)
                    lead.anchored = false;
            }
            else if (!bagPlaced) {
                bagLoc = QPoint(2 * px + pW2 / 2 - bagImg.width() / 2, py - 3 * bagImg.height() / 4 - (mapBottom - py) / 10);
                bagPlaced = true;
                QPointF handAdder = limbs[3].final.A;
                if (LorR)
                    handAdder.setX(-1.0 * handAdder.x());
                lead.anchor = QPoint(2 * px, py - (mapBottom - py) / 10) + handAdder;
                lead.tracking = bagLoc + QPoint(bagImg.width() / 2, bagImg.height() / 5);
                lead.anchors[0] = lead.tracking.toPoint();
                QPointF avg = (lead.anchor + lead.tracking) / 2.0;
                lead.numSegments = 20;
                for (int i = 1; i < lead.numSegments; ++i) {
                    lead.segments[i].posNow = lead.segments[i].posOld = avg;
                }

            }
            break;
    }
    QRect pRect = QRect(px - pW2, py - pH, 2 * pW, pH + 2);
    QPoint offset = QPoint(-px, (mapBottom - py) / 10);
    int dist = 1000;
    int which = 0;
    switch (event->key()) {
        case Qt::Key_Space:
            for (int i = 0; i < lead.numSegments; ++i)
                if (pRect.contains(lead.segments[i].posNow.toPoint() + offset)) {
                    climbing = true;
                    if (abs(py - (lead.segments[i].posNow.y() + offset.y())) < dist) {
                        which = i;
                        dist = abs(py - (lead.segments[i].posNow.y() + offset.y()));
                    }
                }
            if (climbing) {
                px = lead.segments[which].posNow.x() + offset.x();
                py = lead.segments[which].posNow.y() + offset.y();
                whichSeg = which;
            }
            break;
        case Qt::Key_1:
        case Qt::Key_2:
        case Qt::Key_3:
        case Qt::Key_4:
        case Qt::Key_5:
        case Qt::Key_6:
        case Qt::Key_7:
        case Qt::Key_8:
        case Qt::Key_9:
            wIndex = event->key() - Qt::Key_1;
            weapon = shotType(wIndex);
            if (wIndex >= 5)
                wIndex = 4;
            break;
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Control && !event->isAutoRepeat())
        showBounds = !showBounds;
    switch (event->key()) {
        case Qt::Key_A:
        case Qt::Key_Left:
            if (lastKey == Qt::Key_Left) {
                keyDown = false;
            }
            break;
        case Qt::Key_D:
        case Qt::Key_Right:
            if (lastKey == Qt::Key_Right) {
                keyDown = false;
            }
            break;
    }
}

void MainWindow::hairSolver(vector<Rope> *vec, int start, int end) {
    for (int i = start; i < end; ++i)
        (*vec)[i].solve();
}

void MainWindow::ropeSolver(Rope *rope) {
    rope->solve();
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::MouseButton::LeftButton && (!lead.anchored || (!bagPlaced && lead.anchored)))
        mousePressed = true;
    else if (event->button() == Qt::MouseButton::RightButton && !bagPlaced)
        rightBtn = true;
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::MouseButton::LeftButton)
        mousePressed = false;
    else if (event->button() == Qt::MouseButton::RightButton) {
        rightBtn = false;
        throwingStatus = 1;
        float y = 0;
        QPointF back = bagArc.back();
        while (y < bagImg.height() && bagArc.size() > 2) {
            y += (bagArc.back() - bagArc[bagArc.size() - 2]).y();
            back = bagArc.back();
            bagArc.pop_back();
        }
        bagArc.push_back(back);
        reverse(bagArc.begin(), bagArc.end());
        bagPlaced = true;
        lead.anchored = true;
        bagLoc = bagArc.back().toPoint();
        QPointF handAdder = limbs[3].final.A;
        if (LorR)
            handAdder.setX(-1.0 * handAdder.x());
        lead.anchor = QPoint(2 * px, py - (mapBottom - py) / 10) + handAdder;
        lead.anchors.clear();
        lead.pushAnchor(bagLoc);
        QPointF avg = (lead.anchor + lead.tracking) / 2.0;
        lead.numSegments = 20;
        for (int i = 0; i <= lead.numSegments; ++i)
            lead.segments[i].posNow = lead.segments[i].posOld = avg;
    }
}

int MainWindow::onSegment(QPoint p, QPoint q, QPoint r) {
    return (q.x() <= max(p.x(), r.x()) && q.x() >= min(p.x(), r.x()) && q.y() <= max(p.y(), r.y()) && q.y() >= min(p.y(), r.y()));
}

int MainWindow::orient(QPoint p, QPoint q, QPoint r) {
    int val = (q.y() - p.y()) * (r.x() - q.x()) - (q.x() - p.x()) * (r.y() - q.y());
    if (val == 0)
        return 0;  // collinear
    return (val > 0)? 1: 2; // clock or counterclock wise
}

int MainWindow::doIntersect(QPoint p1, QPoint q1, QPoint p2, QPoint q2) {
    int o1 = orient(p1, q1, p2);
    int o2 = orient(p1, q1, q2);
    int o3 = orient(p2, q2, p1);
    int o4 = orient(p2, q2, q1);
    if (o1 != o2 && o3 != o4)
        return 1;
    if (o1 == 0 && onSegment(p1, p2, q1))
        return 1;
    if (o2 == 0 && onSegment(p1, q2, q1))
        return 1;
    if (o3 == 0 && onSegment(p2, p1, q2))
        return 1;
    if (o4 == 0 && onSegment(p2, q1, q2))
        return 1;
    return 0;
}
