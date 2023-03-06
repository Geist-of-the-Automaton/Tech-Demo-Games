#include "mainwindow.h"

#include <QApplication>


#define point pair<float,float>
#define line pair<pair<float,float>,pair<float,float>>

static void print(line l) {
    cout << l.first.first << endl;
    cout << l.first.second << endl;
    cout << l.second.first << endl;
    cout << l.second.second << endl;
}

int main(int argc, char *argv[])
{
    /*
    float lines[2][4] = {{0},{0}};
    float *x1 = &lines[0][0];
    float *y1 = &lines[0][1];
    float *x2 = &lines[0][2];
    float *y2 = &lines[0][3];
    for (float i : lines[0])
        cout << i << endl;
    cout << endl;
    *x1 = 56;
    *y1 = 76;
    *x2 = 2356;
    *y2 = 6;
    lines[0][2] = 43745.54;
    for (float i : lines[0])
        cout << i << endl;
    cout << endl;
    */



    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
