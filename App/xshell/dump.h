#ifndef __DUMP_H__ 
#define __DUMP_H__


#if defined(_WIN32)||defined(_WIN64)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>


#include <string>
using std::string;

typedef void (__stdcall *OnException)(struct _EXCEPTION_POINTERS* pException);

int DumpOpenDebugReport(OnException _OnException);
void DumpCloseDebugReport();
int DumpDmpFile(const string& strFileName,struct _EXCEPTION_POINTERS* pException);
int DumpStackFile(const string& strFileName,struct _EXCEPTION_POINTERS* pException);

string GetStackInfo(HANDLE hProcess,HANDLE hThread,PCONTEXT pContext);

#endif

#endif//__DUMP_H__



/*

time_t t=time(NULL);
tm _tm={0};
localtime_s(&_tm,&t);
char szTime[32]={0};
_snprintf(szTime,31,"%04d%02d%02d%02d%02d%02d",_tm.tm_year+1900,_tm.tm_mon+1,_tm.tm_mday,_tm.tm_hour,_tm.tm_min,_tm.tm_sec);

DumpDmpFile(szTime,pException);
DumpStackFile(szTime,pException);

TerminateProcess(GetCurrentProcess(),-1);
*/