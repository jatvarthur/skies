#include "winos.h"
#include "resource.h"
#include "game.h"
#include "renderos.h"
#include "keybrdos.h"


const float DELTA_STEP = 1.0f / 30.0f;

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

    LARGE_INTEGER counterFreq;
    int fpsCounter = 0;
    int64_t fpsDelta = 0;
    LARGE_INTEGER frameStart, frameEnd;
    float delta = DELTA_STEP;

    g_hInstance = hInstance;
    int exitCode = EXIT_FAILURE;

    if (!gameInit()) {
        goto exitAfterGame;
    }

    if (!createWindow (hInstance, nCmdShow)) {
        goto exitAfterGame;
    }

    if (!renderInit(g_hWnd, g_windowWidth, g_windowHeight)) {
        goto exitAfterGame;
    }

    if (!gameAwake()) {
        goto exitAfterRender;
    }
    
    QueryPerformanceFrequency(&counterFreq);
    QueryPerformanceCounter(&frameStart);

    while (processMessages()) {
        if (IsIconic(g_hWnd)) {
            WaitMessage();
        } else {
            if (delta >= DELTA_STEP) {
                keyBeforeFrame();
                gameStep(delta);
                delta = 0.0f;
                keyAfterFrame();
            }
            render();
            QueryPerformanceCounter(&frameEnd);

            int64_t tickDelta = frameEnd.QuadPart - frameStart.QuadPart;
            delta += tickDelta;
            if (delta > DELTA_STEP * 10) delta = DELTA_STEP;

            fpsCounter += 1;
            fpsDelta += tickDelta;
            if (fpsDelta > counterFreq.QuadPart) {
                g_fps = fpsCounter;
                fpsCounter = 0;
                fpsDelta = 0;
            }
            frameStart = frameEnd;
        }
    }
    gameShutdown();
    exitCode = EXIT_SUCCESS;

exitAfterRender:
    renderClean();
exitAfterGame:
    gameClean();

    return exitCode;
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
