#include "winos.h"
#include "render.h"
#include "renderos.h"


struct RenderContext {
	HWND hWnd;
	ULONG_PTR gToken;
	HFONT hFont;
	HDC hOffscreenDC;
	HBITMAP hOffscreenBuffer;

	int width;
	int height;
	Symbol* screenBuffer;
};

static RenderContext* pCtx = nullptr;

static const COLORREF TERMINAL_COLORS[] = {
	// dark shades
	RGB(  1,  1,   1),
	RGB(134,  30,  21),
	RGB( 29,  90,  36),
	RGB(130, 101,   0),
	RGB(  0,  56,  94),
	RGB( 57,  19,  54),
	RGB( 13,  95, 125),
	RGB(102, 102, 102),
	// normal shades
	RGB( 31,  31,  31),
	RGB(222,  56,  43),
	RGB( 57, 181,  74),
	RGB(255, 199,   6),
	RGB(  0, 111, 184),
	RGB(118,  38, 113),
	RGB( 44, 181, 233),
	RGB(153, 153, 153),
	// light shades
	RGB( 61,  61,  61),
	RGB(242, 172,  76),
	RGB(139, 218, 149),
	RGB(255, 228, 136),
	RGB( 23, 163, 255),
	RGB(175,  56, 167),
	RGB(168, 225, 247),
	RGB(204, 204, 204),
	// bright shades
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
	for ( ; *s != '\0' && x < pCtx->width; ++p, ++s, ++x) {
		p->ch = *s;
		p->fg = fg;
		p->bg = bg;
	}

	return 1;
}

int drawImage(int xDst, int yDst, int xSrc, int ySrc, int wSrc, int hSrc, const SkiiImage& image)
{
	assert(pCtx != nullptr);
	assert(image.isValid());
	assert(xDst >= 0 && xDst < pCtx->width);
	assert(yDst >= 0 && yDst < pCtx->height);
	assert(xSrc >= 0);
	assert(ySrc >= 0);
	assert(wSrc > 0 && xSrc + wSrc <= image.width());
	assert(hSrc > 0 && ySrc + hSrc <= image.height());

	int hOut = min(pCtx->height - yDst, hSrc);
	int wOut = min(pCtx->width - xDst, wSrc);
	for (int j = 0; j < hOut; ++j) {
		Symbol* pdest = &pCtx->screenBuffer[(yDst + j) * pCtx->width + xDst];
		for (int i = xSrc; i < xSrc + wOut; ++i, ++pdest) {
			*pdest = image.at(i, ySrc + j);
		}
	}

	return 1;
}


static int CALLBACK enumFontsProc(const LOGFONTW* lplf, const TEXTMETRIC* lptm, 
	DWORD dwType, LPARAM lpData)
{
	if (lplf->lfHeight == FONTH && lplf->lfWidth == FONTW) {
		*((LOGFONT*)lpData) = *lplf;
		return 0;
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
	pCtx->hOffscreenDC = CreateCompatibleDC(hDC);

	LOGFONTW font;
	findFont(hDC, &font);
	pCtx->hFont = CreateFontIndirectW(&font);
	pCtx->hFont = (HFONT)SelectObject(pCtx->hOffscreenDC, pCtx->hFont);
	pCtx->hOffscreenBuffer = CreateCompatibleBitmap(hDC, width * FONTW, height * FONTH);
	pCtx->hOffscreenBuffer = (HBITMAP)SelectObject(pCtx->hOffscreenDC, pCtx->hOffscreenBuffer);
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
	pCtx->hFont = (HFONT)SelectObject(pCtx->hOffscreenDC, pCtx->hFont);
	pCtx->hOffscreenBuffer = (HBITMAP)SelectObject(pCtx->hOffscreenDC, pCtx->hOffscreenBuffer);
	DeleteObject(pCtx->hFont);
	DeleteObject(pCtx->hOffscreenBuffer);
	DeleteDC(pCtx->hOffscreenDC);

	delete[] pCtx->screenBuffer;
	delete pCtx;
	pCtx = nullptr;
}

const int MAX_BATCH = 255;

static void renderScreen(HDC hdc)
{
	int curFg = -1;
	int curBg = -1;
	for (int j = 0; j < pCtx->height; ++j) {
		schar_t batch[MAX_BATCH];
		int batchSize = 0;
		int batchX = 0;
		for (int i = 0; i < pCtx->width; ++i) {
			int bufIdx = j * pCtx->width + i;
			int fg = pCtx->screenBuffer[bufIdx].fg;
			int bg = pCtx->screenBuffer[bufIdx].bg;

			assert(fg >= 0 && fg < NELEMS(TERMINAL_COLORS));
			assert(bg >= 0 && bg < NELEMS(TERMINAL_COLORS));

			if (((curBg != bg || curFg != fg) && batchSize > 0) || batchSize >= MAX_BATCH) {
				TextOutA(hdc, batchX * FONTW, j * FONTH, batch, batchSize);
				batchSize = 0;
				batchX = i;
			}

			batch[batchSize++] = pCtx->screenBuffer[bufIdx].ch;

			if (curBg != bg) {
				SetBkColor(hdc, TERMINAL_COLORS[bg]);
				curBg = bg;
			}

			if (curFg != fg) {
				SetTextColor(hdc, TERMINAL_COLORS[fg]);
				curFg = fg;
			}
		}

		if (batchSize > 0) {
			TextOutA(hdc, batchX * FONTW, j * FONTH, batch, batchSize);
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
	renderScreen(pCtx->hOffscreenDC);
	renderFps(pCtx->hOffscreenDC);

	RECT rect;
	GetClientRect(pCtx->hWnd, &rect);	
	HDC hDC = GetDC(pCtx->hWnd);
	BitBlt(hDC, 0, 0, rect.right, rect.bottom, pCtx->hOffscreenDC, 0, 0, SRCCOPY);
	ReleaseDC(pCtx->hWnd, hDC);
}
