// zyrhpic.cpp: 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "desktopCapture.h"
#include <windows.h>
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include "app.h"
#include "xshell/xshell.hpp"
#include<iostream>
#include<thread>
#include <stdio.h>
#include<string>
#include<vector>




int main(int argc, char* argv[])
{
// 	std::thread *a = new std::thread(SendMessageOne);
// 	a->join();
	Xshell<App> xh("app", "app");
	return xh.Run(argc, argv);
    return 0;
}

