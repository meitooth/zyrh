#include "stdafx.h"
#include "CaptureScreen.h"
#include "utils.h"
void CaptureScreen::ScreenInit(bool IsMain)
{    
	
	nScreenWidth =  SCREEN_WIDTH*GetScreenCount();
	nScreenHeight = SCREEN_HEIGHT;
	

    pbi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    pbi.bmiHeader.biWidth = nScreenWidth;
    pbi.bmiHeader.biHeight = nScreenHeight;
    pbi.bmiHeader.biPlanes = 1;
    pbi.bmiHeader.biBitCount = 24;
    pbi.bmiHeader.biCompression = BI_RGB;
    stride = ((nScreenWidth * 24 + 31) & 0xffffffe0) / 8;  // 24Î»Í¼ÏñÉ¨ÃèÏß¿í¶È
    scan0 = (PRGBTRIPLE)malloc(stride * nScreenHeight);    // Í¼ÏñÊý¾Ý»º³åÇø£¬Ó¦ÊÍ·Å
	RECT rect;
	rect.left = rect.top = 0;
	rect.bottom = nScreenHeight;
	ScreenSetWindow(&rect);
	
}

void CaptureScreen::ScreenFree()
{
    free(scan0); 
}

bool CaptureScreen::ScreenSetWindow(LPRECT lpRect)
{
     top_x = lpRect->left;
     top_y = lpRect->top;
     bot_x = lpRect->right;
     bot_y = lpRect->bottom;
    if(top_x >= bot_x)
    return false;
    if(top_y >= bot_y)
    return false;
    if(top_x<0 || bot_x>nScreenWidth)
    return false;
    if(top_y<0 || bot_y>nScreenHeight)
    return false;

    rHeight = bot_y-top_y;
    rWidth =  bot_x-top_x;    
    return true;
}

bool CaptureScreen::ScreenCopy(BYTE *dstBuf)
{    
	HWND hDesktopWnd = GetDesktopWindow();
	HDC hDesktopDC = GetDC(hDesktopWnd);
	HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
	HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, nScreenWidth, nScreenHeight);
	SelectObject(hCaptureDC, hCaptureBitmap);
	BitBlt(hCaptureDC, 0, 0, nScreenWidth, nScreenHeight, hDesktopDC, 0, 0, SRCCOPY | NOMIRRORBITMAP);
	DeleteDC(hCaptureDC);

	GetDIBits(hDesktopDC, hCaptureBitmap, 0, nScreenHeight, scan0, &pbi, DIB_RGB_COLORS);
	ReleaseDC(hDesktopWnd, hDesktopDC);
	DeleteObject(hCaptureBitmap);

	BYTE *srcBuf = (BYTE*)scan0;

	int j;
	srcBuf += (nScreenHeight - 1 - top_y)*stride + top_x * 3;// scan0µ¹Ðò
	for (j = 0;j < rHeight;j++)
	{
		memcpy(dstBuf, srcBuf, rWidth * 3);
		dstBuf += rWidth * 3;
		srcBuf -= stride;
	}
	return true;
}
bool CaptureScreen::ScreenCopySecond(BYTE *dstBuf)
{
	HWND hDesktopWnd = GetDesktopWindow();
	HDC hDesktopDC = GetDC(hDesktopWnd);
	HDC hCaptureDC = CreateCompatibleDC(hDesktopDC);
	HBITMAP hCaptureBitmap = CreateCompatibleBitmap(hDesktopDC, nScreenWidth, nScreenHeight);
	SelectObject(hCaptureDC, hCaptureBitmap);
	BitBlt(hCaptureDC, 0, 0, nScreenWidth, nScreenHeight, hDesktopDC, 0, 0, SRCCOPY | NOMIRRORBITMAP);
	DeleteDC(hCaptureDC);

	GetDIBits(hDesktopDC, hCaptureBitmap, 0, nScreenHeight, scan0, &pbi, DIB_RGB_COLORS);
	ReleaseDC(hDesktopWnd, hDesktopDC);
	DeleteObject(hCaptureBitmap);

	BYTE *srcBuf = (BYTE*)scan0;

	int j;
	srcBuf += (nScreenHeight - 1 - top_y)*stride + top_x * 3;// scan0µ¹Ðò
	for (j = 0;j < rHeight;j++)
	{
		memcpy(dstBuf, srcBuf, rWidth * 3);
		dstBuf += rWidth * 3;
		srcBuf -= stride;
	}
	return true;
}