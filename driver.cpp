#include "winos.h"
#include "resource.h"
#include "game.h"
#include "renderos.h"
#include "keybrdos.h"


#define MAX_LOADSTRING 100

HINSTANCE g_hInstance;
HWND g_hWnd;
int g_fps;

static LPCWSTR WINDOW_CLASS_NAME = L"SkiesConsoleWndClass";

BOOL createWindow(HINSTANCE, int);
LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM);
BOOL processMessages();

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    g_hInstance = hInstance;

    if (!gameInit()) {
        return FALSE;
    }

    if (!createWindow (hInstance, nCmdShow)) {
        return FALSE;
    }

    if (!renderInit(g_hWnd, g_windowWidth, g_windowHeight)) {
        return FALSE;
    }

    LARGE_INTEGER counterFreq;
    QueryPerformanceFrequency(&counterFreq);

    int fpsCounter = 0;
    int64_t fpsDelta = 0;

    LARGE_INTEGER frameStart, frameEnd;
    QueryPerformanceCounter(&frameStart);

    while (processMessages()) {
        if (IsIconic(g_hWnd)) {
            WaitMessage();
        } else {
            keyBeforeFrame();
            // todo: delta
            gameStep(0);
            render();
            QueryPerformanceCounter(&frameEnd);

            int64_t tickDelta = frameEnd.QuadPart - frameStart.QuadPart;
            fpsCounter += 1;
            fpsDelta += tickDelta;
            if (fpsDelta > counterFreq.QuadPart) {
                g_fps = fpsCounter;
                fpsCounter = 0;
                fpsDelta = 0;
            }
            frameStart = frameEnd;

            keyAfterFrame();
        }
    }

    gameClean();
    renderClean();

    return 0;
}

BOOL createWindow(HINSTANCE hInstance, int nCmdShow)
{
    WCHAR title[MAX_LOADSTRING];
    LoadStringW(hInstance, IDS_APP_TITLE, title, MAX_LOADSTRING);

    WNDCLASSEXW wcex;
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = wndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_SKIES));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = WINDOW_CLASS_NAME;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    ATOM aWndClass = RegisterClassExW(&wcex);
    if (!aWndClass) {
        return FALSE;
    }

    DWORD style = WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    RECT rect = { 0, 0, g_windowWidth * FONTW, g_windowHeight * FONTH };
    AdjustWindowRect(&rect, style, FALSE);
    g_hWnd = CreateWindowW((LPCWSTR)aWndClass, title, WS_OVERLAPPED | style,
        CW_USEDEFAULT, 0, rect.right - rect.left, rect.bottom - rect.top, nullptr, nullptr, hInstance, nullptr);

   if (!g_hWnd) {
      return FALSE;
   }

   ShowWindow(g_hWnd, nCmdShow);
   UpdateWindow(g_hWnd);

   return TRUE;
}

BOOL processMessages()
{
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return FALSE;
        } else {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return TRUE;
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    /*case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            EndPaint(hWnd, &ps);
        }
        break;*/
    case WM_KEYDOWN:
        keySetState((Key)(wParam & KEY_LAST), true);
        break;
    case WM_KEYUP:
        keySetState((Key)(wParam & KEY_LAST), false);
        break;
    case WM_CHAR:
        // todo: support character buffer
        break;
    case WM_ERASEBKGND:
        return TRUE;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
