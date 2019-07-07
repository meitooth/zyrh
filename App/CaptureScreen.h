#ifndef _CAPTURESCREEN_H
#define _CAPTURESCREEN_H

#include <windows.h>
class  CaptureScreen
{
public:
    CaptureScreen()
    {};
    ~CaptureScreen(void)
    {};

    void ScreenInit(bool IsMain = true);
    void ScreenFree();
    bool ScreenSetWindow(LPRECT lpRect); 
    bool ScreenCopy(BYTE *dstBuf); 
	bool ScreenCopySecond(BYTE *dstBuf);

    int rHeight;    //要获得的图像尺寸
    int rWidth;
	int nScreenWidth;
	int nScreenHeight;
private:    
    
    PRGBTRIPLE scan0;
    BITMAPINFO pbi;
    int stride;    
    int top_x;
    int top_y;
    int bot_x;
    int bot_y;
};

#endif