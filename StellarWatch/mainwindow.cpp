#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    menuTheme.setSource(QUrl::fromLocalFile("menu.wav"));
    menuTheme.setVolume(0.125f);
    menuTheme.setLoopCount(menuTheme.Infinite);
    menuOver.setSource(QUrl::fromLocalFile("menuOver.wav"));
    menuOver.setVolume(0.5f);
    menuSelect.setSource(QUrl::fromLocalFile("menuSelect.wav"));
    menuSelect.setVolume(0.5f);
    menuBack.setSource(QUrl::fromLocalFile("menuBack.wav"));
    menuBack.setVolume(0.5f);
    //setWindowFlag(Qt::FramelessWindowHint, true);
    QImage mitem;
    mitem.load("menuLister.png");
    for (int j = 0; j < mitem.height(); ++j) {
        QRgb *line = reinterpret_cast<QRgb *>(mitem.scanLine(j));
        for (int i = 0; i < mitem.width(); ++i) {
            if (line[i] == 0xFFFFFFFF)
                line[i] = 0x00000000;
            else {
                QColor color(line[i]);
                color.setAlphaF(static_cast<float>(i) / static_cast<float>(2 * mitem.width()));
                line[i] = color.rgba();
            }
        }
    }
    mlister = QPixmap::fromImage(mitem);
    mitem.load("menuListerSelected.png");
    for (int j = 0; j < mitem.height(); ++j) {
        QRgb *line = reinterpret_cast<QRgb *>(mitem.scanLine(j));
        for (int i = 0; i < mitem.width(); ++i) {
            if (line[i] == 0xFFFFFFFF)
                line[i] = 0x00000000;
            else {
                QColor color(line[i]);
                color.setAlphaF(static_cast<float>(i) / static_cast<float>(2 * mitem.width()));
                line[i] = color.rgba();
            }
        }
    }
    slister = QPixmap::fromImage(mitem);
    bg = QImage(1280 * 2, 720, QImage::Format_ARGB32_Premultiplied);
    setFixedSize(bg.width() / 2, bg.height());
    float div1 = bg.width() / 2;
    float div2 = bg.height() / 4;
    for (int j = 0; j < bg.height(); ++j) {
        QRgb *line = reinterpret_cast<QRgb *>(bg.scanLine(j));
        for (int i = 0; i < bg.width(); ++i) {
            int num = static_cast<int>(127.0 * (cos(static_cast<float>(i) * pi / div1) + 1.0));
            int num2 = static_cast<int>(65.0 * (cos(static_cast<float>(j) * pi / div2) + 1.0));
            QColor color(num, num2, 255);
            line[i] = color.rgba();
        }
    }
    for (int i = 0; i < 3; ++i)
        scoreTexts.push_back("---\t-\t0");
    bgtOff = 0.0;
    tinc = 0.0;
    tplane.load("plane2.png");
    tplane = tplane.scaledToWidth(tplane.width() * 20);
    titleImg.load("title.png");
    QImage temp;
    temp.load("title.png");
    for (int i = 0; i < titleImg.width(); ++i)
        for (int j = 0; j < titleImg.height(); ++j) {
            QColor color = temp.pixelColor(i, j);
            if (color.alpha() == 0)
                continue;
            color.setHsl(color.hslHue() + 20, color.hslSaturation(), color.lightness());
            temp.setPixelColor(i, j, color);
        }
    bgTitle = QPixmap::fromImage(temp);
    pauseImg.load("pause.png");
    temp.load("pause.png");
    for (int i = 0; i < pauseImg.width(); ++i)
        for (int j = 0; j < pauseImg.height(); ++j) {
            QColor color = temp.pixelColor(i, j);
            if (color.alpha() == 0)
                continue;
            color.setHsl(color.hslHue() + 20, color.hslSaturation(), color.lightness());
            temp.setPixelColor(i, j, color);
        }
    bgPause = QPixmap::fromImage(temp);
    wasdOn.load("useWASDon.png");
    wasdOff.load("useWASDoff.png");
    wasdSlider.load("slider.png");
    wasdImg.load("useWASD.png");
    clearScores.load("clearScores.png");
    for (int i = 0; i <= 2; ++i) {
        int num = i + 1;
        temp.load(QString(to_string(num).c_str()) + ".png");
        cntDownF.push_back(QPixmap::fromImage(temp));
        for (int i = 0; i < temp.width(); ++i)
            for (int j = 0; j < temp.height(); ++j) {
                QColor color = temp.pixelColor(i, j);
                if (color.alpha() == 0)
                    continue;
                color.setHsl(color.hslHue() + 20, color.hslSaturation(), color.lightness());
                temp.setPixelColor(i, j, color);
            }
        cntDownB.push_back(QPixmap::fromImage(temp));
    }
    for (int i = 0; i < 4; ++i)
        outs.push_back(0);
    temp = QImage(bg.width() / 2, bg.height(), QImage::Format_ARGB32_Premultiplied);
    temp.fill(0xFF000000);
    blackBg = QPixmap::fromImage(temp);
    for (int i = 0; i < starSizes; ++i) {
        QImage star(2 * (starSizes - i), 2 * (starSizes - i), QImage::Format_ARGB32_Premultiplied);
        int num = 200 * (starSizes - i) / starSizes;
        star.fill(QColor(num, num, num));
        starImgs.push_back(QPixmap::fromImage(star));
    }
    stars = vector(starSizes, vector <QPointF> ());
    for (int i = 0; i < starSizes; ++i) {
        for (int j = 0; j < numStarsPerSize; ++j)
            stars[i].push_back(QPoint(rand() % (blackBg.width() - i), rand() % (blackBg.height() - i)));
    }
    laserImg.load("laser.png");
    for (int i = 0; i <= 2; ++i) {
        temp.load("plane" + QString(to_string(i + 1).c_str()) + ".png");
        QColor check (181, 230, 29);
        gunLocs.push_back(vector<QPoint>());
        for (int y = 0; y < temp.height(); ++y)
            for (int x = 0; x < temp.width(); ++x)
                if (temp.pixelColor(x, y) == check)
                    gunLocs[i].push_back(3 * (QPoint(x, y) - QPoint(temp.width() / 2, 0)));
        ships.push_back(QPixmap::fromImage(temp.scaledToWidth(temp.width() * scale)));
    }
    playerShip = 0;
    playerLoc = QPoint(blackBg.width() / 2, blackBg.height() - (ships[0].height() + 1));
    up = down = left = right = 0;
    cooldown = 0;
    gun = 0;
    wasdSliderOff = 0;
    usesMouse = true;
    isFiring = false;
    frameless = true;
    setWindowFlag(Qt::FramelessWindowHint, true);
    shift = 0;
    textMaps[title] = {"Start", "High Scores", "Settings", "Exit"};
    textMaps[game] = {};
    textMaps[scores] = {"<- Back"};
    textMaps[settings] = {"<- Back"};
    textMaps[choose] = {"Quit"};
    textMaps[pause] = {"Quit", "Resume"};
    textMaps[trans] = {};
    textMaps[pauseBack] = {};
    type = title;
    texts = textMaps[type];
    connect(&timer, SIGNAL(timeout()), this, SLOT(nextFrame()));
    timer.start(1000 / fps);
    menuTheme.play();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::nextFrame() {
    long long ttemp = t;
    logTime();
    QPoint point = mapFromGlobal(QCursor::pos());
    if (type == trans) {
        cover += transDir;
        bgtOff += 1.0;
        tinc += 0.1;
        if (cover < 0) {
            cover = 0.0;
            transDir = 0.02;
            type = title;
            texts = textMaps[type];
        }
        else if (cover > 1.0) {
            cover = 1.0;
            transDir = -0.02;
            type = game;
            texts = textMaps[type];
        }
    }
    if (type == pause || type == pauseBack) {
        bgtOff += 1.0;
        tinc += 0.1;
        if (type == pauseBack) {
            cntDown += ttemp;
            if (cntDown >= 1500) {
                type = game;
                texts = textMaps[type];
            }
        }
    }
    if (type != game && type != pause && type != choose && type != trans) {
        bgtOff += 1.0;
        tinc += 0.1;
        if (usesMouse && wasdSliderOff > 0)
            wasdSliderOff -= 10;
        else if (!usesMouse && wasdSliderOff < wasdOn.width() - wasdSlider.width())
            wasdSliderOff += 10;
        bool check = shift == 0 || shift == bg.width() / 2;
        for (int j = 0; j < bg.height(); ++j) {
            QRgb *line = reinterpret_cast<QRgb *>(bg.scanLine(j));
            if (check) {
                QRgb end = line[bg.width() - 1];
                for (int i = bg.width() - 1; i > 0; --i)
                    line[i] = line[i - 1];
                line[0] = end;
            }
            int j2 = j == 0 ? bg.height() - 1 : j - 1;
            QRgb *line2 = reinterpret_cast<QRgb *>(bg.scanLine(j2));
            for (int i = 0; i < bg.width(); ++i) {
                QRgb temp = line[i];
                line[i] = line2[i];
                line2[i] = temp;
            }
        }
    }
    if (type == trans || type == game) {
        for (int i = 0; i < starSizes; ++i) {
            QPointF pAdd = QPointF(0, static_cast<float>(starSizes - i + 1) * 0.5);
            for (int j = 0; j < numStarsPerSize; ++j) {
                stars[i][j] += pAdd;
                if (stars[i][j].y() >= blackBg.height()) {
                    stars[i][j].setY(-starSizes);
                    stars[i][j].setX(rand() % blackBg.width());
                }
            }
        }
        float mult = 1.0 + static_cast<float>(2 - playerShip) * 0.25;
        QPoint wasdPt = 1000 * QPoint(left + right, up + down);
        if (wasdPt == QPoint(0, 0))
            wasdPt = playerLoc;
        QPoint to = usesMouse ? point : wasdPt;
        float xd = to.x() - playerLoc.x();
        float yd = to.y() - playerLoc.y();
        float dist = sqrt(xd * xd + yd * yd);
        float speed = static_cast<float>(mult * playerSpeed);
        if (dist <= speed)
            playerLoc = to;
        else if (true) {
            float angle = atan2(yd, xd);
            float s = sin(angle);
            float c = cos(angle);
            float xnew = speed * c;
            float ynew = speed * s;
            playerLoc += QPoint(xnew, ynew);
        }
        int w = ships[playerShip].width() / 2;
        int h = ships[playerShip].height();
        if (playerLoc.x() < w)
            playerLoc.setX(w);
        else if (playerLoc.x() > blackBg.width() - w)
            playerLoc.setX(blackBg.width() - w);
        if (playerLoc.y() < 0)
            playerLoc.setY(0);
        else if (playerLoc.y() > blackBg.height() - h)
            playerLoc.setY(blackBg.height() - h);
    }
    else {
        // process game to screen if trans, pause, choose, or gameover
        over = -1;
        for (int i = 0; i < outs.size(); ++i) {
            QRect rect = QRect(hoff, voff + mlister.height() * (vSep * i + 1), mlister.width(), mlister.height());
            if (rect.contains(point) && i < texts.size()) {
                if (outs[i] < movement) {
                    outs[i] += 2;
                    if (outs[i] > movement)
                        outs[i] = movement;
                }
                over = i;
            }
            else if (outs[i] > 0)
                --outs[i];
        }
        if (over != -1 && lastOver != over)
            menuOver.play();
        lastOver = over;
    }
    if (isFiring && cooldown == 0) {
        QPoint first = playerLoc + gunLocs[playerShip][gun];
        projectiles.push_back(projectile(first.x(), first.y(), laser));
        cooldown = baseFireRate - 2 * playerShip;
        gun = (gun + 1) % gunLocs[playerShip].size();
        if (playerShip == 2 && gun % 2 == 1) {
            QPoint second = playerLoc + gunLocs[playerShip][gun];
            projectiles.push_back(projectile(second.x(), second.y(), laser));
            second = (first + second) / 2;
            projectiles.push_back(projectile(second.x(), second.y(), laser));
            gun = (gun + 1) % gunLocs[playerShip].size();
        }
    }
    else if (cooldown > 0)
        --cooldown;
    for (int i = 0; i < projectiles.size(); ++i) {
        projectiles[i].advance();
        if (projectiles[i].y < 0) {
            projectiles.erase(projectiles.begin() + i);
            --i;
        }
    }
    bool check = point.y() <= (frameless ? 0 : 30) && point.x() >= 0 && point.x() < blackBg.width();
    if (frameless != check) {
        hide();
        frameless = check;
        setWindowFlag(Qt::FramelessWindowHint, !check);
        show();
    }
    repaint();
    logTime(t);
    //getTime();
}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter qp(this);
    QFont qf = qp.font();
    qf.setPointSize(qf.pointSize() * 2);
    qp.setFont(qf);
    if (type == game || type == pause || type == pauseBack) {
        qp.drawPixmap(0, 0, blackBg);
        for (int i = 0; i < starSizes; ++i)
            for (QPointF point : stars[i])
                qp.drawPixmap(point, starImgs[i]);
        qp.drawPixmap(playerLoc - QPoint(ships[playerShip].width() / 2, 0), ships[playerShip]);
        for (int i = 0; i < projectiles.size(); ++i)
            qp.drawPixmap(projectiles[i].x, projectiles[i].y, laserImg);
        if (type != game) {
            float x = 10.0 * (cos(pi * tinc / 10.0)), y = 0;
            float s = sin(bgtOff * deg2rad);
            float c = cos(bgtOff * deg2rad);
            float xnew = x * c - y * s;
            float ynew = x * s + y * c;
            int which = 2 - (cntDown / 500);
            QPixmap pma = type == pause ? pauseImg : cntDownF[which];
            QPixmap pmb = type == pause ? bgPause : cntDownB[which];
            qp.translate(0, (blackBg.height() - pma.height()) / 2);
            qp.rotate(x / 10.0);
            y = 10.0 * (sin(pi * tinc / 10.0));
            qp.drawPixmap(xnew + x, ynew + y, pmb);

            qp.drawPixmap(x, y, pma);
            qp.resetTransform();
            for (int i = 0; i < texts.size(); ++i) {
                QPixmap toDraw = i == over ? slister : mlister;
                qp.drawPixmap(hoff + outs[i], voff + toDraw.height() * (vSep * i + 1), toDraw);
                QColor color(255 - outs[i] * 25 / movement, 255 - outs[i] * 25 / movement, 255 - outs[i] * 200 / movement);
                QPen pen = qp.pen();
                pen.setColor(color);
                qp.setPen(pen);
                qp.drawText(hoff + outs[i] + toDraw.width() / 2, voff + toDraw.height() * (vSep * i + 1) + 2 * toDraw.height() / 3, texts[i]);
            }
        }
        return;
    }
    else if (type == trans) {
        QImage img = bg.copy(0, 0, bg.width() / 2, bg.height());
        qp.drawPixmap(0, 0, blackBg);
        for (int i = 0; i < starSizes; ++i)
            for (QPointF point : stars[i])
                qp.drawPixmap(point, starImgs[i]);
        qp.drawPixmap(playerLoc - QPoint(ships[playerShip].width() / 2, 0), ships[playerShip]);
        qp.setOpacity(1.0 - cover);
        qp.drawImage(0, 0, img);
        for (int i = 0; i < texts.size(); ++i) {
            QPixmap toDraw = i == over ? slister : mlister;
            qp.drawPixmap(hoff + outs[i], voff + toDraw.height() * (vSep * i + 1), toDraw);
            QColor color(255 - outs[i] * 25 / movement, 255 - outs[i] * 25 / movement, 255 - outs[i] * 200 / movement);
            QPen pen = qp.pen();
            pen.setColor(color);
            qp.setPen(pen);
            qp.drawText(hoff + outs[i] + toDraw.width() / 2, voff + toDraw.height() * (vSep * i + 1) + 2 * toDraw.height() / 3, texts[i]);
        }
    }
    else if (type == title || type == scores || type == settings) {
        shift = type == title ? max(shift - 10, 0) : min(shift + 10, bg.width() / 2);
        QImage img = bg.copy(shift, 0, bg.width() / 2, bg.height());
        qp.drawImage(0, 0, img);
        float x = 10.0 * (cos(pi * tinc / 10.0)), y = 0;
        float s = sin(bgtOff * deg2rad);
        float c = cos(bgtOff * deg2rad);

        // rotate point
        float xnew = x * c - y * s;
        float ynew = x * s + y * c;
        qp.rotate(x / 10.0);
        y = 10.0 * (sin(pi * tinc / 10.0));
        qp.drawPixmap(xnew + x, ynew + y, bgTitle);

        qp.drawPixmap(x, y, titleImg);
        qp.resetTransform();
        if (shift != bg.width() / 2) {
            qp.translate(2 * img.width() / 3, 3 * img.height() / 5);
            qp.rotate(y / 2);
            qp.translate(-tplane.width() / 2, -tplane.height() / 2);
            qp.scale(1.0 + x / 100.0, 1.0 + x / 100.0);
            qp.setOpacity(1.0 - static_cast<float>(shift) / static_cast<float>(bg.width() / 2));
            qp.drawPixmap(0, 0, tplane);
            qp.resetTransform();
            qp.setOpacity(1.0);
        }
        if (type == scores) {
            QPen pen = qp.pen();
            pen.setColor(0xFFFFFFFF);
            qp.setPen(pen);
            qp.translate(6 * img.width() / 13, 2 * img.height() / 5);
            qp.setOpacity(static_cast<float>(shift) / static_cast<float>(bg.width() / 2));
            qp.scale(3, 3);
            for (int i = 0; i < scoreTexts.size(); ++i)
                qp.drawText(10.0 * cos(pi * (tinc + 5.0 * i) / 15.0), mlister.height() * i + y, scoreTexts[i]);
            qp.resetTransform();
        }
        else if (type == settings) {
            QPen pen = qp.pen();
            qp.translate(img.width() / 2, 2 * img.height() / 5);
            qp.setOpacity(static_cast<float>(shift) / static_cast<float>(bg.width() / 2));
            //qp.scale(3, 3);
            qp.drawPixmap(10.0 * cos(pi * (tinc) / 15.0), y, wasdImg);

            if (wasdSliderOff == 0 || wasdSliderOff == wasdOn.width() - wasdSlider.width()) {
                qp.drawImage(10.0 * cos(pi * (tinc + 5.0 * 1) / 15.0), wasdImg.height() * 1 + y, usesMouse ? wasdOff : wasdOn);
                qp.drawPixmap(10.0 * cos(pi * (tinc + 5.0 * 1) / 15.0) + wasdSliderOff, wasdImg.height() * 1 + y, wasdSlider);
                qp.drawPixmap(10.0 * cos(pi * (tinc + 5.0 * 3) / 15.0), wasdImg.height() * 3 + y, clearScores);
            }
            else {
                int cut = wasdOff.width() - wasdSliderOff;
                QImage temp = wasdOff.copy(wasdSliderOff, 0, cut, wasdSlider.height());
                qp.drawImage(10.0 * cos(pi * (tinc + 5.0 * 1) / 15.0) + wasdSliderOff, wasdImg.height() * 1 + y, temp);
                temp = wasdOn.copy(0, 0, wasdSliderOff + wasdSlider.width() / 2, wasdSlider.height());
                qp.drawImage(10.0 * cos(pi * (tinc + 5.0 * 1) / 15.0), wasdImg.height() * 1 + y, temp);
                qp.drawPixmap(10.0 * cos(pi * (tinc + 5.0 * 1) / 15.0) + wasdSliderOff, wasdImg.height() * 1 + y, wasdSlider);
                qp.drawPixmap(10.0 * cos(pi * (tinc + 5.0 * 3) / 15.0), wasdImg.height() * 3 + y, clearScores);
            }
            qp.resetTransform();
        }
        for (int i = 0; i < texts.size(); ++i) {
            QPixmap toDraw = i == over ? slister : mlister;
            qp.drawPixmap(hoff + outs[i], voff + toDraw.height() * (vSep * i + 1), toDraw);
            QColor color(255 - outs[i] * 25 / movement, 255 - outs[i] * 25 / movement, 255 - outs[i] * 200 / movement);
            QPen pen = qp.pen();
            pen.setColor(color);
            qp.setPen(pen);
            qp.drawText(hoff + outs[i] + toDraw.width() / 2, voff + toDraw.height() * (vSep * i + 1) + 2 * toDraw.height() / 3, texts[i]);
        }
    }
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    QPoint point = mapFromGlobal(QCursor::pos());
    if (type == game) {
        isFiring = true;
        return;
    }
    if (type == settings) {
        float y = 10.0 * (sin(pi * tinc / 10.0));
        point -= QPoint(blackBg.width() / 2, 2 * blackBg.height() / 5);
        QRect rect = QRect(10.0 * cos(pi * (tinc + 5.0 * 1) / 15.0), wasdImg.height() * 1 + y, wasdImg.width(), wasdImg.height());
        QRect rect2 = QRect(10.0 * cos(pi * (tinc + 5.0 * 3) / 15.0), wasdImg.height() * 3 + y, wasdImg.width(), wasdImg.height());
        if (rect.contains(point)) {
            usesMouse = !usesMouse;
            menuSelect.play();
        }
        else if (rect2.contains(point)) {
            ;//clearScores
            menuSelect.play();
        }
        else
            point += QPoint(blackBg.width() / 2, 2 * blackBg.height() / 5);
    }
    for (int i = 0; i < texts.size(); ++i) {
        QRect rect = QRect(hoff, voff + mlister.height() * (vSep * i + 1), mlister.width(), mlister.height());
        if (!rect.contains(point))
            continue;
        bool whichSnd = true;
        if ((type == scores || type == settings) && i == 0) {
            type = title;
            whichSnd = false;
        }
        else if ((type == choose || type == pause) && i == 0) {
            type = trans;
            whichSnd = false;
        }
        else if (type == title && i == 0)
            type = trans;
        else if (type == title && i == 1)
            type = scores;
        else if (type == title && i == 2)
            type = settings;
        else if (type == title && i == 3)
            QApplication::exit();
        else if (type == pause && i == 0) {
            type = trans;
            whichSnd = false;
        }
        else if (type == pause && i == 1) {
            type = pauseBack;
            cntDown = 0;
        }
        texts = textMaps[type];
        if (whichSnd)
            menuSelect.play();
        else
            menuBack.play();
        break;
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) {
    if (type == game)
        isFiring = false;
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    int key = event->key();
    if (key == Qt::Key_W)
        up = -playerSpeed;
    else if (key == Qt::Key_S)
        down = playerSpeed;
    else if (key == Qt::Key_A)
        left = -playerSpeed;
    else if (key == Qt::Key_D)
        right = playerSpeed;
    else if (key == Qt::Key_Left && playerShip > 0)
        --playerShip;
    else if (key == Qt::Key_Right && playerShip < ships.size())
        ++playerShip;
    else if (key == Qt::Key_Space)
        isFiring = true;
    else if (key == Qt::Key_Alt)
        isFiringSec = true;
    else if (key == Qt::Key_Q)
        usesMouse = !usesMouse;
    else if (type == game && (key == Qt::Key_Escape || key == Qt::Key_Tab || key == Qt::Key_Shift || key == Qt::Key_CapsLock || key == Qt::Key_Enter)) {
        type = pause;
        texts = textMaps[type];
        menuBack.play();
    }
}

void MainWindow::keyReleaseEvent(QKeyEvent *event) {
    int key = event->key();
    if (key == Qt::Key_W)
        up = 0;
    else if (key == Qt::Key_S)
        down = 0;
    else if (key == Qt::Key_A)
        left = 0;
    else if (key == Qt::Key_D)
        right = 0;
    else if (key == Qt::Key_Space)
        isFiring = false;
    else if (key == Qt::Key_Alt)
        isFiringSec = false;
}

