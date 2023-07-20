//
// Created by piz2a on 2023-07-18.
//

#include "MainWindow.h"



void MainWindow::create(HWND hwnd_, HINSTANCE g_hInst_) {
    hwnd = hwnd_;
    g_hInst = g_hInst_;
}


void MainWindow::TouchProc(const std::vector<MovingWindow*>& vMovingWindow, int nCode, WPARAM wParam, LPARAM lParam) {
    /*HWND photoHwnd = CreateWindow("PhotoWindow", "우리들의 추억", WS_OVERLAPPEDWINDOW,
                                  CW_USEDEFAULT, CW_USEDEFAULT, 800, 600, (HWND)nullptr, (HMENU)nullptr, (HINSTANCE)nullptr, nullptr);
    ShowWindow(photoHwnd, SW_SHOWNORMAL);*/
    for (MovingWindow* movingWindow : vMovingWindow) {
        if (movingWindow->velocityCmp(1000) == 1) continue;
        movingWindow->multiplyVelocity(20.0);
        movingWindow->setRestitution(0.9);
    }
/*
    DWORD dwThreadID;
    HANDLE hThread = CreateThread(nullptr, 0, [vMovingWindow](LPVOID lpParam) WINAPI -> DWORD {
        Sleep(3000);
        for (MovingWindow movingWindow : vMovingWindow) movingWindow.setRestitution(1.0);
        return 0;
    }, nullptr, 0, &dwThreadID);
    if (hThread == nullptr) ExitProcess(3);*/
}


void MainWindow::onWMCreate(UINT iMessage, WPARAM wParam, LPARAM lParam) {
    /*
    HWND hChildWnd = CreateWindow(
            "button",        		// 윈도우 클래스 이름
            "버튼",	                // 윈도우 타이틀
            WS_CHILD | WS_VISIBLE, 	// 윈도우 스타일
            0,                      // 윈도우 보일 때 x 좌표
            0,                      // 윈도우 보일 때 y 좌표
            windowWidth,            // 윈도우 폭
            100,                    // 윈도우 높이
            hwnd,         	        // 부모 윈도우
            (HMENU)ID_OK_BTN,       // 컨트롤 ID
            g_hInst,           	    // 인스턴스 핸들
            (LPVOID)nullptr);       // 여분의 데이터

    if (!hChildWnd) return -1;*/
    /*hBitmap = (HBITMAP) LoadImage(g_hInst, "C:/Users/jiho/Desktop/gift.jpg", IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    std::cout << "bitmap: " << hBitmap << std::endl;*/
}


void MainWindow::onWMPaint(UINT iMessage, WPARAM wParam, LPARAM lParam) {
    LPCTSTR text = "아무 키나 눌러보세요";
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);
    TextOut(hdc, 0, 0, text, lstrlen(text));

    /*
    BITMAP bitmap;
    HDC hdcMem = CreateCompatibleDC(hdc);
    HGDIOBJ oldBitmap = SelectObject(hdcMem, hBitmap);

    GetObject(hBitmap, sizeof(bitmap), &bitmap);
    BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, oldBitmap);
    DeleteDC(hdcMem);
     */

    EndPaint(hwnd, &ps);
}


void MainWindow::onWMCommand(UINT iMessage, WPARAM wParam, LPARAM lParam) {
    /*
    if (LOWORD(wParam) == ID_OK_BTN)
    {
        // SetWindowPos(hWnd, 0, 500, 500, 300, 300, SWP_NOREDRAW | SWP_NOZORDER);
        // SetWindowLongPtr(hWnd, GWL_STYLE, dwNewStyles);
        // RedrawWindow(hWnd, 0, 0, RDW_INVALIDATE | RDW_FRAME);
        MessageBox(hwnd, "버튼이 클릭되었습니다.", "메시지", MB_OK);
    }
    */
}


void MainWindow::onWMDestroy(UINT iMessage, WPARAM wParam, LPARAM lParam) {
    // DeleteObject(hBitmap);
}
