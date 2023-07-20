//
// Created by piz2a on 2023-07-18.
//

#ifndef BOUNCINGWINDOWS_MOVINGWINDOW_H
#define BOUNCINGWINDOWS_MOVINGWINDOW_H


#include <windows.h>
#include <cmath>
#include <iostream>

#define DELAY_TIME  5             // microseconds
#define GRAVITY 300               // pixels/second^2
#define INITIAL_SPEED 20          // pixels/second


class MovingWindow {
public:
    MovingWindow(HWND hwnd_, int desktopWidth_, int desktopHeight_, double vx_, double vy_);
    void geometry();
    void geometry(int x_, int y_, int width_, int height_);
    void accelerate();
    void move();
    void refreshRestitution();
    void multiplyVelocity(double ratio) { vx *= ratio; vy *= ratio; }
    void setRestitution(double value) { restitution = value; }
    int velocityCmp(double v) { double temp = vx * vx + vy * vy - v * v; return temp == 0 ? 0 : (temp > 0 ? 1 : -1); }
    HWND getHwnd() { return hwnd; }
private:
    int desktopWidth, desktopHeight;
    HWND hwnd;
    RECT initialRect;
    double restitution = 1.0;
    int width, height;
    double x, y;
    double vx, vy;
};


#endif //BOUNCINGWINDOWS_MOVINGWINDOW_H
