/**********************************************************************
* File:        tessedit.cpp  (Formerly tessedit.c)
* Description: Main program for merge of tess and editor.
* Author:                  Ray Smith
* Created:                 Tue Jan 07 15:21:46 GMT 1992
*
* (C) Copyright 1992, Hewlett-Packard Ltd.
** Licensed under the Apache License, Version 2.0 (the "License");
*/
#include "stdafx.h"
#include "ocr.h"
#include <iostream>



#include <string>
#include "utils.h"
#include <direct.h>
#include <corecrt_io.h>



/**
 * We have 2 possible sources of pagesegmode: a config file and
 * the command line. For backwards compatibility reasons, the
 * default in tesseract is tesseract::PSM_SINGLE_BLOCK, but the
 * default for this program is tesseract::PSM_AUTO. We will let
 * the config file take priority, so the command-line default
 * can take priority over the tesseract default, so we use the
 * value from the command line only if the retrieved mode
 * is still tesseract::PSM_SINGLE_BLOCK, indicating no change
 * in any config file. Therefore the only way to force
 * tesseract::PSM_SINGLE_BLOCK is from the command line.
 * It would be simpler if we could set the value before Init,
 * but that doesn't work.
 */




void PreloadRenderers(){
	
}



int doocr(const char* image, std::string& strout) {
	







	return 0;
}
int doocrtif(const char* image, std::string& strout, std::string outputname)
{
	return 0;
}

int doocrtifbox(const char* image, std::string& strout, std::string outputname)
{

	return 0;
}
int doocrdigit(const char* image, std::string& strout) {
	return 0;
}
int doocrcoutdown(const char* image, std::string& strout) {
	return 0;
}
int doocrcancel(const char* image, std::string& strout) {
	return 0;
}
int execcmd(const char *cmd)
{
#ifdef WIN32
	PROCESS_INFORMATION info;
	STARTUPINFO si = { 0 };
	DWORD stat;
	char cmds[1024];


	si.cb = sizeof(si);
	sprintf(cmds, "cmd /c %s", cmd);
	if (!CreateProcess(NULL, (LPTSTR)cmds, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL,
		NULL, &si, &info)) return -1;
	WaitForSingleObject(info.hProcess, INFINITE);
	if (!GetExitCodeProcess(info.hProcess, &stat)) stat = -1;
	CloseHandle(info.hProcess);
	CloseHandle(info.hThread);
	return (int)stat;
#else
	trace(3, "execcmd: cmd=%s\n", cmd);

	return system(cmd);
#endif
}
void doocrdigit_cmd(const char* image, std::string& strout)
{


}
void doocrcoutdown_cmd(const char* image, std::string& strout)
{
	

}

void doocr_cmd(const char* image, std::string& strout)
{
	
}