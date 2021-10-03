#include "winos.h"
#include "render.h"
#include "renderos.h"


struct Symbol {
	char ch;
	color_t fg;
	color_t bg;
	uint8_t unused;
};

struct RenderContext {
	HWND hWnd;
	ULONG_PTR gToken;
	HFONT hFont;
	HBITMAP hOffscreenBuffer;

	int width;
	int height;
	Symbol* screenBuffer;
};

static RenderContext* pCtx = nullptr;

static const COLORREF TERMINAL_COLORS[] = {
	RGB(  1,   1,   1),
	RGB(222,  56,  43),
	RGB( 57, 181,  74),
	RGB(255, 199,   6),
	RGB(  0, 111, 184),
	RGB(118,  38, 113),
	RGB( 44, 181, 233),
	RGB(204, 204, 204),
	RGB(128, 128, 128),
	RGB(255,   0,   0),
	RGB(  0, 255,   0),
	RGB(255, 255,   0),
	RGB(  0,   0, 255),
	RGB(255,   0, 255),
	RGB(  0, 255, 255),
	RGB(255, 255, 255)
};

#define NELEMS(a) (sizeof(a) / sizeof(a[0]))

int drawChar(int x, int y, char ch, color_t fg, color_t bg)
{
	assert(pCtx != nullptr);
	assert(x >= 0 && x < pCtx->width);
	assert(y >= 0 && y < pCtx->height);

	Symbol* p = &pCtx->screenBuffer[y * pCtx->width + x];
	p->ch = ch;
	p->fg = fg;
	p->bg = bg;

	return 1;
}

int drawString(int x, int y, const char* s, color_t fg, color_t bg)
{
	assert(pCtx != nullptr);
	assert(x >= 0 && x < pCtx->width);
	assert(y >= 0 && y < pCtx->height);
	assert(s != nullptr);

	Symbol* p = &pCtx->screenBuffer[y * pCtx->width + x];
	while (char c = *(s++)) {
		p->ch = c;
		p->fg = fg;
		p->bg = bg;
		p += 1;
	}

	return 1;
}

static int CALLBACK enumFontsProc(const LOGFONTW* lplf, const TEXTMETRIC* lptm, 
	DWORD dwType, LPARAM lpData)
{
	if (lplf->lfHeight == FONTH && lplf->lfWidth == FONTW) {
		*((LOGFONT*)lpData) = *lplf;
		return 1;
	}
	return 1;
}

static bool findFont(HDC hdc, LOGFONT* result) {
	LOGFONT logfont;
	ZeroMemory(&logfont, sizeof logfont);
	logfont.lfCharSet = DEFAULT_CHARSET;
	logfont.lfPitchAndFamily = FIXED_PITCH | FF_DONTCARE;
	lstrcpy(logfont.lfFaceName, L"Terminal");

	result->lfHeight = 0;
	EnumFontFamiliesExW(hdc, &logfont, enumFontsProc, (LPARAM)result, 0);
	return result->lfHeight != 0;
}

bool renderInit(HWND hWnd, int width, int height)
{	
	pCtx = new RenderContext;
	pCtx->hWnd = hWnd;
	
	HDC hDC = GetDC(pCtx->hWnd);
	LOGFONTW font;
	findFont(hDC, &font);
	pCtx->hFont = CreateFontIndirectW(&font);
	pCtx->hOffscreenBuffer = CreateCompatibleBitmap(hDC, width * FONTW, height * FONTH);
	ReleaseDC(pCtx->hWnd, hDC);

	int bufSize = height * width;
	pCtx->width = width;
	pCtx->height = height;
	pCtx->screenBuffer = new Symbol[bufSize];
	for (int i = 0; i < bufSize; ++i) {
		pCtx->screenBuffer[i].bg = COLOR_BLACK;
		pCtx->screenBuffer[i].fg = COLOR_WHITE;
		pCtx->screenBuffer[i].ch = ' ';
	}

	return true;
}

void renderClean()
{
	DeleteObject(pCtx->hFont);
	DeleteObject(pCtx->hOffscreenBuffer);

	delete[] pCtx->screenBuffer;
	delete pCtx;
	pCtx = nullptr;
}

static void renderScreen(HDC hdc)
{
	for (int j = 0; j < pCtx->height; ++j) {
		for (int i = 0; i < pCtx->width; ++i) {
			int bufIdx = j * pCtx->width + i;
			int fg = pCtx->screenBuffer[bufIdx].fg;
			int bg = pCtx->screenBuffer[bufIdx].bg;

			assert(fg >= 0 && fg < NELEMS(TERMINAL_COLORS));
			assert(bg >= 0 && bg < NELEMS(TERMINAL_COLORS));

			SetTextColor(hdc, TERMINAL_COLORS[fg]);
			SetBkColor(hdc, TERMINAL_COLORS[bg]);
			TextOutA(hdc, i * FONTW, j * FONTH, &pCtx->screenBuffer[bufIdx].ch, 1);
		}
	}
}

extern int g_fps;

static void renderFps(HDC hdc)
{
	char buf[40] = { 0 };
	snprintf(buf, 40, "%dfps", g_fps);

	SetTextColor(hdc, 0xFFFFFF);
	SetBkColor(hdc, 0xBB);
	TextOutA(hdc, 10, 10, buf, strlen(buf));
}

void render()
{
	RECT rect;
	GetClientRect(pCtx->hWnd, &rect);
	
	HDC hDC = GetDC(pCtx->hWnd);
	HDC hMemDC = CreateCompatibleDC(hDC);
	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, pCtx->hOffscreenBuffer);
	HFONT hOldFont = (HFONT)SelectObject(hMemDC, pCtx->hFont);

	PatBlt(hMemDC, 0, 0, pCtx->width * FONTW, pCtx->height * FONTH, BLACKNESS);
	renderScreen(hMemDC);

	SelectObject(hMemDC, hOldFont);
	renderFps(hMemDC);

	BitBlt(hDC, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);

	SelectObject(hMemDC, hOldBitmap);
	ReleaseDC(pCtx->hWnd, hMemDC);
	ReleaseDC(pCtx->hWnd, hDC);
}
