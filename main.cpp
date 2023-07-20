#include <windows.h>
#include <vector>
#include <set>
#include <random>
#include <iostream>
#include <tlhelp32.h>
#include <codecvt>
#include <algorithm>
#include <chrono>

#include "MovingWindow.h"
#include "MainWindow.h"


using namespace std::chrono;

std::set<std::wstring> excludingApps = {
        L"explorer.exe", L"KakaoTalk.exe", L"chrome.exe", L"clion64.exe",
        L"SystemSettings.exe", L"svchost.exe", L"ApplicationFrameHost.exe", L"TextInputHost.exe", L"Adobe Desktop Service.exe"
};
const int windowWidth = 260, windowHeight = 260;

int desktopWidth, desktopHeight;
int nWindow = 0;
bool running = false;
MainWindow mainWindow;
std::vector<MovingWindow*> vMovingWindow;
HINSTANCE g_hInst;
HHOOK hKeyboardHook;
HHOOK hMouseHook;
LPCTSTR lpszClass = "BouncingWindows";


void GetEntries(std::vector<PROCESSENTRY32W> &pids);
BOOL CALLBACK EnumWindowsProcMy(HWND, LPARAM);
DWORD WINAPI ThreadFunction(LPVOID);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszCmdParam,
                     int nCmdShow) {
    RECT desktop;
    const HWND hDesktop = GetDesktopWindow();
    GetWindowRect(hDesktop, &desktop);
    desktopWidth = desktop.right;
    desktopHeight = desktop.bottom;

    HWND hwnd;
    MSG Message;
    WNDCLASS WndClass;

    WndClass.cbClsExtra = 0;
    WndClass.cbWndExtra = 0;
    WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    WndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
    WndClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    WndClass.hInstance = hInstance;
    WndClass.lpfnWndProc = (WNDPROC)WndProc;
    WndClass.lpszClassName = lpszClass;
    WndClass.lpszMenuName = nullptr;
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClass(&WndClass);     //메인윈도우 클래스 등록

    hwnd = CreateWindow(lpszClass,			                 //윈도우 클래스 이름
                        "Don't Touch!",			             //윈도우 타이틀
                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,    //윈도우 스타일
                        (desktopWidth - windowWidth) / 2,
                        (desktopHeight - windowHeight) / 2,	 //윈도우가 보일 때 X Y좌표
                        windowWidth, windowHeight,			 //윈도우의 폭과 높이
                        (HWND)nullptr,						 //부모 윈도우 핸들
                        (HMENU)nullptr,						 //윈도우가 가지는 메뉴 핸들
                        hInstance,							 //인스턴스 핸들
                        nullptr);							 //여분의 데이터
    if (hwnd == nullptr) return 1;
    ShowWindow(hwnd, nCmdShow);
    SetWindowLong(hwnd, GWL_STYLE,
                  GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
    mainWindow.create(hwnd, g_hInst);

    std::vector<PROCESSENTRY32W> entries;
    GetEntries(entries);
    EnumWindows(EnumWindowsProcMy, reinterpret_cast<LPARAM>(&entries));

    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC) [](int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION && (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN))
            mainWindow.TouchProc(vMovingWindow, nCode, wParam, lParam);
        return CallNextHookEx(hKeyboardHook, nCode,wParam,lParam);
    }, g_hInst, NULL);
    /*hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC) [](int nCode, WPARAM wParam, LPARAM lParam) {
        if (nCode == HC_ACTION && (wParam == WM_LBUTTONDOWN || wParam == WM_RBUTTONDOWN))
            mainWindow.TouchProc(vMovingWindow, nCode, wParam, lParam);
        return CallNextHookEx(hMouseHook, nCode,wParam,lParam);
    }, g_hInst, NULL);*/

    DWORD dwThreadID;
    HANDLE hThread = CreateThread(nullptr, 0, ThreadFunction, nullptr, 0, &dwThreadID);

    if (hThread == nullptr)
        ExitProcess(3);

    while (GetMessage(&Message, nullptr, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    return Message.wParam;
}


void GetEntries(std::vector<PROCESSENTRY32W> &entries) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    // Do use a proper RAII type for this so it's robust against exceptions and code changes.
    auto cleanupSnap = [snap] { CloseHandle(snap); };

    PROCESSENTRY32W entry;
    entry.dwSize = sizeof entry;

    if (!Process32FirstW(snap, &entry)) {
        cleanupSnap();
        return;
    }

    do {
        if (std::none_of(excludingApps.begin(), excludingApps.end(),
                        [entry](const std::wstring& bouncingAppName) {
            /* std::wcout << std::wstring(entry.szExeFile) << std::endl; */ return entry.szExeFile == bouncingAppName;
        })) {
            entries.emplace_back(entry/*.th32ProcessID*/);
        }
    } while (Process32NextW(snap, &entry));
    cleanupSnap();

    /*
    std::cout << "ProcessIDs: ";
    for (PROCESSENTRY32W entry : entries) std::cout << entry.th32ProcessID << " ";
    std::cout << std::endl;
     */
}


BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam) {
    if (!IsWindowVisible(hwnd)) return TRUE;

    const auto &entries = *reinterpret_cast<std::vector<PROCESSENTRY32W>*>(lParam);
    DWORD winId;
    GetWindowThreadProcessId(hwnd, &winId);
    std::wstring exePath;
    for (PROCESSENTRY32W entry : entries) {
        if (entry.th32ProcessID == winId) exePath = entry.szExeFile;
    }
    if (std::none_of(std::begin(entries), std::end(entries),
                     [winId](PROCESSENTRY32W entry) { return winId == entry.th32ProcessID; })) {
        return TRUE;  // if winId not in pids: return
    }
    std::wcout << "Process ID: " << winId << " / exePath: " << exePath << std::endl;

    /*
    std::wstring title(GetWindowTextLength(hwnd) + 1, L'\0');
    GetWindowTextW(hwnd, &title[0], title.size()); //note: C++11 only

    std::cout << "Found window" << "\n";
    std::wcout << "Title: " << title << "\n";
     */

    WINDOWPLACEMENT placement;
    placement.length = sizeof(placement);
    GetWindowPlacement(hwnd, &placement);

    BOOL minimized = (placement.showCmd & SW_SHOWMINIMIZED) != 0;
    if (minimized) {
        placement.showCmd = SW_SHOWNORMAL;
        SetWindowPlacement(hwnd, &placement);
    }

    RECT rect;
    GetWindowRect(hwnd, &rect);

    // Random degree
    std::random_device rd;
    std::mt19937 mt(rd());
    std::uniform_real_distribution<double> dist(0.0, 3.141592);
    double degree = dist(mt);
    auto mw = new MovingWindow(hwnd, desktopWidth, desktopHeight, INITIAL_SPEED * cos(degree), INITIAL_SPEED * sin(degree));

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    mw->geometry((desktopWidth - width) / 2, (desktopHeight - height) / 2, width, height);
    mw->move();
    PostMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);

    vMovingWindow.push_back(mw);
    nWindow++;
    return TRUE;
}


DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    running = true;
    auto moveWindow = [] {
        for (auto iter = vMovingWindow.begin(); iter != vMovingWindow.end(); iter++) {
            if ((*iter)->getHwnd() == nullptr) {
                vMovingWindow.erase(iter);
                continue;
            }
            (*iter)->refreshRestitution();
            (*iter)->accelerate();
            (*iter)->move();
        }
    };
    milliseconds test_start = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    moveWindow();
    milliseconds test_end = duration_cast<milliseconds>(system_clock::now().time_since_epoch());
    milliseconds moving_time = test_end - test_start;
    int delay_time = DELAY_TIME - moving_time.count();
    delay_time = delay_time > 0 ? delay_time : 1;
    std::cout << delay_time  << std::endl;
    // Bounce
    while (running) {
        Sleep(delay_time);
        moveWindow();
    }
    // Recovery
    for (auto movingWindow : vMovingWindow) {
        movingWindow->geometry();
    }

    return 0;
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT iMessage, WPARAM wParam, LPARAM lParam) {
    switch (iMessage) {
        case WM_CREATE:
            mainWindow.onWMCreate(iMessage, wParam, lParam);
            break;
        case WM_PAINT:
            mainWindow.onWMPaint(iMessage, wParam, lParam);
            break;
        case WM_COMMAND:
            mainWindow.onWMCommand(iMessage, wParam, lParam);
            break;
        case WM_DESTROY:
            running = false;
            mainWindow.onWMDestroy(iMessage, wParam, lParam);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, iMessage, wParam, lParam);
    }
    return 0;
}
