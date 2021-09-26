#include <cassert>
#include "winos.h"
#include <gdiplus.h>
#include "render.h"
#include "renderos.h"

using namespace Gdiplus;

struct Symbol {
	char ch;
	int8_t fg;
	int8_t bg;
};

struct RenderContext {
	HWND hWnd;
	ULONG_PTR gToken;
	Image* pFontTex;

	int width;
	int height;
	Symbol* screenBuffer;
};

static RenderContext* pCtx = nullptr;


static LPCWSTR FONT_FILE_NAME = L".\\assets\\terminal8x8_aa_ro.png";

int putCh(int x, int y, char ch, color_t fg, color_t bg)
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

int putStr(int x, int y, const char* s, color_t fg, color_t bg)
{

	return 0;
}

bool renderInit(HWND hWnd, int width, int height)
{
	ULONG_PTR token;
	GdiplusStartupInput startupInput;
	if (GdiplusStartup(&token, &startupInput, NULL) != Status::Ok) {
		return false;
	}
	
	pCtx = new RenderContext;
	pCtx->hWnd = hWnd;
	pCtx->gToken = token;
	pCtx->width = width;
	pCtx->height = height;
	pCtx->screenBuffer = new Symbol[height * width];
	pCtx->pFontTex = new Image(FONT_FILE_NAME); // todo: check
	
	return true;
}

void render()
{
	RECT rect;
	GetClientRect(pCtx->hWnd , &rect);
	Bitmap memBitmap(rect.right, rect.bottom);
	Graphics g(&memBitmap);
	g.Clear(Color::AliceBlue);
	g.SetPageUnit(UnitPixel);
	RectF bounds(0, 0, float(16 * FONTW), float(16 * FONTH));
	g.DrawImage(pCtx->pFontTex, bounds);

	HDC hDC = GetDC(pCtx->hWnd);
	Graphics s(hDC);
	s.DrawImage(&memBitmap, 0, 0);
	//BitBlt(hDC, 0, 0, rect.right, rect.bottom, g.GetHDC(), 0, 0, SRCCOPY);
	ReleaseDC(pCtx->hWnd, hDC);
}

void renderClean()
{
	GdiplusShutdown(pCtx->gToken);

	delete [] pCtx->screenBuffer;
	delete pCtx;
	pCtx = nullptr;
}
