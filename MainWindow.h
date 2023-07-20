//
// Created by piz2a on 2023-07-18.
//

#ifndef BOUNCINGWINDOWS_MAINWINDOW_H
#define BOUNCINGWINDOWS_MAINWINDOW_H


#include <windows.h>
#include <iostream>
#include <vector>
#include "MovingWindow.h"

#pragma comment(lib, "winmm.lib")

// #define ID_OK_BTN	2000


class MainWindow {
public:
    void create(HWND hwnd_, HINSTANCE g_hInst_);
    void TouchProc(const std::vector<MovingWindow*>& vMovingWindow, int nCode, WPARAM wParam, LPARAM lParam);
    void onWMCreate(UINT iMessage, WPARAM wParam, LPARAM lParam);
    void onWMPaint(UINT iMessage, WPARAM wParam, LPARAM lParam);
    void onWMCommand(UINT iMessage, WPARAM wParam, LPARAM lParam);
    void onWMDestroy(UINT iMessage, WPARAM wParam, LPARAM lParam);
private:
    HWND hwnd{};
    HINSTANCE g_hInst{};
    HBITMAP hBitmap{};
};


#endif //BOUNCINGWINDOWS_MAINWINDOW_H
