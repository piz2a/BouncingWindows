//
// Created by piz2a on 2023-07-18.
//

#include "MainWindow.h"



void MainWindow::create(HWND hwnd_, HINSTANCE g_hInst_) {
    hwnd = hwnd_;
    g_hInst = g_hInst_;
}


void MainWindow::TouchProc(const std::vector<MovingWindow*>& vMovingWindow, int nCode, WPARAM wParam, LPARAM lParam) {
    for (MovingWindow* movingWindow : vMovingWindow) {
        if (movingWindow->velocityCmp(1000) == 1) continue;
        movingWindow->multiplyVelocity(20.0);
        movingWindow->setRestitution(0.9);
    }
}


void MainWindow::onWMCreate(UINT iMessage, WPARAM wParam, LPARAM lParam) {
}


void MainWindow::onWMPaint(UINT iMessage, WPARAM wParam, LPARAM lParam) {
    LPCTSTR text = "아무 키나 눌러보세요";
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    TextOut(hdc, 0, 0, text, lstrlen(text));
    EndPaint(hwnd, &ps);
}


void MainWindow::onWMCommand(UINT iMessage, WPARAM wParam, LPARAM lParam) {
}


void MainWindow::onWMDestroy(UINT iMessage, WPARAM wParam, LPARAM lParam) {
}
