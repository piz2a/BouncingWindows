//
// Created by piz2a on 2023-07-18.
//

#include "MovingWindow.h"


MovingWindow::MovingWindow(HWND hwnd_, int desktopWidth_, int desktopHeight_, double vx_, double vy_) {\
    desktopWidth = desktopWidth_, desktopHeight = desktopHeight_;
    hwnd = hwnd_, vx = vx_, vy = vy_;
    GetWindowRect(hwnd, &initialRect);
    geometry();
}

void MovingWindow::geometry() {  // initialize geometry
    geometry(initialRect.left, initialRect.top, initialRect.right - initialRect.left, initialRect.bottom - initialRect.top);
}

void MovingWindow::geometry(int x_, int y_, int width_, int height_) {
    x = x_, y = y_, width = width_, height = height_;
}

void MovingWindow::accelerate() {
    if (x < 0) {
        x = 0;
        vx *= -restitution;
    }
    else if (x > desktopWidth - width) {
        x = desktopWidth - width;
        vx *= -restitution;
    }
    if (y < 0) {
        y = 0;
        vy *= -restitution;
    }
    else if (y > desktopHeight - height) {
        y = desktopHeight - height;
        vy *= -restitution;
    }
    double delta_vy = GRAVITY * DELAY_TIME / 1000.0;
    // std::cout << vx << " " << vy << std::endl;
    x += vx;
    y += vy + delta_vy / 2;
    vy += delta_vy;
}

void MovingWindow::move() {
    SetWindowPos(hwnd, nullptr, (int)round(x), (int)round(y), width, height, SWP_NOREDRAW | SWP_NOZORDER);
    // SetWindowLongPtr(hWnd, GWL_STYLE, dwNewStyles);
    // RedrawWindow(hwnd, 0, 0, RDW_INVALIDATE | RDW_FRAME);
}

void MovingWindow::refreshRestitution() {
    if (restitution < 1.0 && velocityCmp(INITIAL_SPEED) == -1) {
        setRestitution(1.0);
    }
}
