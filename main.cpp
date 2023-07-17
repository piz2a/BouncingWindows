#include <windows.h>
#include <vector>
#include <random>


#define ID_OK_BTN	2000
#define DELAY_TIME  2             // microseconds
#define GRAVITY 300               // pixels/second^2
#define INITIAL_SPEED 50         // pixels/second
#define RESTITUTION  0.9          // 반발 계수


class MovingWindow;

int desktopWidth, desktopHeight;
int windowWidth = 260, windowHeight = 260;
int nWindow = 0;
std::vector<MovingWindow*> vMovingWindow;
BOOL running = FALSE;
HINSTANCE g_hInst;
LPCTSTR lpszClass = "HelloWinAPI";

DWORD WINAPI ThreadFunction(LPVOID);
BOOL CALLBACK EnumWindowsProcMy(HWND, LPARAM);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

class MovingWindow {
public:
    MovingWindow(HWND hwnd_, double vx_, double vy_);
    void geometry();
    void geometry(int x_, int y_, int width_, int height_);
    void accelerate();
    void move();
    HWND getHwnd() { return hwnd; }
private:
    HWND hwnd;
    RECT initialRect;
    int width, height;
    double x, y;
    double vx, vy;
};

MovingWindow::MovingWindow(HWND hwnd_, double vx_, double vy_) {
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
        vx *= -RESTITUTION;
    }
    else if (x > desktopWidth - width) {
        x = desktopWidth - width;
        vx *= -RESTITUTION;
    }
    if (y < 0) {
        y = 0;
        vy *= -RESTITUTION;
    }
    else if (y > desktopHeight - height) {
        y = desktopHeight - height;
        vy *= -RESTITUTION;
    }
    double delta_vy = GRAVITY * DELAY_TIME / 1000.0;
    x += vx;
    y += vy + delta_vy / 2;
    vy += delta_vy;
}

void MovingWindow::move() {
    SetWindowPos(hwnd, 0, round(x), round(y), width, height, SWP_NOREDRAW | SWP_NOZORDER);
    // SetWindowLongPtr(hWnd, GWL_STYLE, dwNewStyles);
    // RedrawWindow(hwnd, 0, 0, RDW_INVALIDATE | RDW_FRAME);
}


int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpszCmdParam,
                     int nCmdShow)
{
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
    WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    WndClass.hInstance = hInstance;
    WndClass.lpfnWndProc = (WNDPROC)WndProc;
    WndClass.lpszClassName = lpszClass;
    WndClass.lpszMenuName = nullptr;
    WndClass.style = CS_HREDRAW | CS_VREDRAW;
    RegisterClass(&WndClass);     //메인윈도우 클래스 등록

    hwnd = CreateWindow(lpszClass,			//윈도우클래스 이름
                        "Windows Show",			        //윈도우타이틀
                        WS_OVERLAPPEDWINDOW | WS_VISIBLE,   //윈도우스타일
                        (desktopWidth - windowWidth) / 2, (desktopHeight - windowHeight) / 2,							//윈도우가 보일때 X Y좌표
                        windowWidth, windowHeight,							//윈도우의 폭과 높이
                        (HWND)NULL,							//부모윈도우 핸들
                        (HMENU)NULL,						//윈도우가 가지는 메뉴핸들
                        hInstance,							//인스턴스핸들
                        NULL);								//여분의 데이터

    if (hwnd == NULL)
        return 0;

    ShowWindow(hwnd, nCmdShow);

    EnumWindows(EnumWindowsProcMy, NULL);

    DWORD dwThreadID;
    HANDLE hThread = CreateThread(nullptr, 0, ThreadFunction, nullptr, 0, &dwThreadID);

    if (hThread == nullptr)
        ExitProcess(3);

    while (GetMessage(&Message, 0, 0, 0)) {
        TranslateMessage(&Message);
        DispatchMessage(&Message);
    }

    return Message.wParam;
}

DWORD WINAPI ThreadFunction(LPVOID lpParam) {
    running = TRUE;
    // Bounce
    while(running) {
        Sleep(DELAY_TIME);
        for (auto iter = vMovingWindow.begin(); iter != vMovingWindow.end(); iter++) {
            if ((**iter).getHwnd() == nullptr) {
                vMovingWindow.erase(iter);
                continue;
            }
            (**iter).accelerate();
            (**iter).move();
        }
    }
    // Recovery
    for (auto & iter : vMovingWindow) {
        (*iter).geometry();
    }

    return 0;
}

BOOL CALLBACK EnumWindowsProcMy(HWND hwnd, LPARAM lParam)
{
    DWORD lpdwProcessId;
    GetWindowThreadProcessId(hwnd, &lpdwProcessId);
    if (lpdwProcessId == lParam) {
        return TRUE;
    }

    if (IsWindowVisible(hwnd)) {
        WINDOWPLACEMENT placement;
        placement.length = sizeof(placement);

        GetWindowPlacement(hwnd, &placement);

        BOOL minimized = (placement.showCmd & SW_SHOWMINIMIZED) != 0;
        if (minimized) {
            placement.showCmd = SW_SHOWNORMAL;
            SetWindowPlacement(hwnd, &placement);
        }

        // Random degree
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_real_distribution<double> dist(0.0, 3.141592);
        double degree = dist(mt);
        auto* mw = new MovingWindow(hwnd, INITIAL_SPEED * cos(degree), INITIAL_SPEED * sin(degree));

        mw->geometry((desktopWidth - windowWidth) / 2, (desktopHeight - windowHeight) / 2, windowWidth, windowHeight);
        mw->move();
        PostMessage(hwnd, WM_SYSCOMMAND, SC_RESTORE, 0);

        vMovingWindow.push_back(mw);
        nWindow++;
    }
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMessage,
                         WPARAM wParam, LPARAM lParam)
{
    LPCTSTR text = "메인윈도우 생성";
    switch (iMessage) {
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            TextOut(hdc, 0, 0, text, lstrlen(text));
            EndPaint(hwnd, &ps);
            return 0;
        }
        case WM_CREATE:
        {
            HWND hChildWnd = CreateWindow(
                    "button",        		// 윈도우 클래스 이름
                    "버튼",	            // 윈도우 타이틀
                    WS_CHILD | WS_VISIBLE, 	// 윈도우 스타일
                    0,                      // 윈도우 보일 때 x 좌표
                    50,                     // 윈도우 보일 때 y 좌표
                    100,                    // 윈도우 폭
                    30,                     // 윈도우 높이
                    hwnd,         	        // 부모 윈도우
                    (HMENU)ID_OK_BTN,       // 컨트롤 ID
                    g_hInst,           	    // 인스턴스 핸들
                    (LPVOID)nullptr);          // 여분의 데이터

            if (!hChildWnd) 	return -1;
            return 0;
        }
        case WM_COMMAND:
        {
            if (LOWORD(wParam) == ID_OK_BTN)
            {
                /*
                SetWindowPos(hWnd, 0, 500, 500, 300, 300, SWP_NOREDRAW | SWP_NOZORDER);
                // SetWindowLongPtr(hWnd, GWL_STYLE, dwNewStyles);
                RedrawWindow(hWnd, 0, 0, RDW_INVALIDATE | RDW_FRAME);
                */
                MessageBox(hwnd, "버튼이 클릭되었습니다.", "메시지", MB_OK);
            }

            return 0;
        }
        case WM_DESTROY:
            running = FALSE;
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, iMessage, wParam, lParam);
}