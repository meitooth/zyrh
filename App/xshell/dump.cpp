#include "dump.h"


#if defined(_WIN32)||defined(_WIN64)

#include <Dbghelp.h>
#pragma comment(lib,"Dbghelp.lib")




OnException g_OnException=NULL;
DWORD g_dwErrorMode=0;










LONG WINAPI UnhandledException_Filter(struct _EXCEPTION_POINTERS* ExceptionInfo)
{
	if(g_OnException)
	{
		g_OnException(ExceptionInfo);
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

void UnhandleInvalidParameter_Filter(const wchar_t * expression, const wchar_t * function, const wchar_t * file, unsigned int line, uintptr_t reserved)
{
	EXCEPTION_RECORD exception_record = {0};
	CONTEXT exception_context = {0};
	EXCEPTION_POINTERS exception={&exception_record,&exception_context};
	RtlCaptureContext(exception.ContextRecord);
	exception.ExceptionRecord->ExceptionCode=STATUS_NONCONTINUABLE_EXCEPTION;

	if(g_OnException)
	{
		g_OnException(&exception);
	}
}

void UnhandlePurecall_Filter()
{
	EXCEPTION_RECORD exception_record = {0};
	CONTEXT exception_context = {0};
	EXCEPTION_POINTERS exception={&exception_record,&exception_context};
	RtlCaptureContext(exception.ContextRecord);
	exception.ExceptionRecord->ExceptionCode=STATUS_NONCONTINUABLE_EXCEPTION;
	
	if(g_OnException)
	{
		g_OnException(&exception);
	}
}












int DumpOpenDebugReport(OnException _OnException)
{
	g_OnException=_OnException;

	g_dwErrorMode=SetErrorMode(SEM_FAILCRITICALERRORS|SEM_NOGPFAULTERRORBOX);
	SetUnhandledExceptionFilter(UnhandledException_Filter);	
	_set_invalid_parameter_handler(UnhandleInvalidParameter_Filter);
	_set_purecall_handler(UnhandlePurecall_Filter);
	return 0;
}

void DumpCloseDebugReport()
{
	SetErrorMode(g_dwErrorMode);
	SetUnhandledExceptionFilter(NULL);
}


int DumpDmpFile(const string& strFileName,struct _EXCEPTION_POINTERS* pException)
{
	HANDLE hFile=CreateFileA((strFileName+".dmp").c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!hFile)
	{
		return -1;
	}

	MINIDUMP_EXCEPTION_INFORMATION exception={0};
	exception.ClientPointers=TRUE;
	exception.ThreadId=GetCurrentThreadId();
	exception.ExceptionPointers=pException;

	if(!MiniDumpWriteDump(GetCurrentProcess(),GetCurrentProcessId(),hFile, (MINIDUMP_TYPE)(MiniDumpNormal|MiniDumpWithThreadInfo),&exception,NULL,NULL))
	{
		CloseHandle(hFile);
		return -2;
	}
	CloseHandle(hFile);
	return 0;
}

int DumpStackFile(const string& strFileName,struct _EXCEPTION_POINTERS* pException)
{
	HANDLE hFile=CreateFileA((strFileName+".txt").c_str(),GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(!hFile)
	{
		return -1;
	}

	HANDLE hProcess=GetCurrentProcess();
	HANDLE hThread=GetCurrentThread();

	string strStackInfo = GetStackInfo(hProcess,hThread,pException->ContextRecord);
	DWORD wr=0;
	if(!WriteFile(hFile,strStackInfo.c_str(),strStackInfo.size()+1,&wr,NULL))
	{
		CloseHandle(hFile);
		return -1;
	}
	CloseHandle(hFile);
	return 0;
}

string GetStackInfo(HANDLE hProcess,HANDLE hThread,PCONTEXT pContext)
{
	STACKFRAME64 sf;
	ZeroMemory(&sf, sizeof(STACKFRAME));
	sf.AddrPC.Offset    = pContext->Eip;
	sf.AddrPC.Mode      = AddrModeFlat;
	sf.AddrStack.Offset = pContext->Esp;
	sf.AddrStack.Mode   = AddrModeFlat;
	sf.AddrFrame.Offset = pContext->Ebp;
	sf.AddrFrame.Mode   = AddrModeFlat;
	
	if(!SymInitialize(hProcess,NULL,TRUE))
	{
		char err[256]={0};
#if defined(_WIN32)||defined(_WIN64)
		_snprintf(err,255,"SymInitialize failed,error=%u",GetLastError());
#else
		snprintf(err,255,"SymInitialize failed,error=%u",GetLastError());
#endif
		return err;
	}

	string strStackInfo;

	while(StackWalk64(IMAGE_FILE_MACHINE_I386,hProcess,hThread,&sf,pContext,NULL,SymFunctionTableAccess64,SymGetModuleBase64,NULL))
	{
		string strStackLine;


		//获取函数名
		ULONG64 buffer[(sizeof(SYMBOL_INFO) +
			MAX_SYM_NAME*sizeof(TCHAR) +
			sizeof(ULONG64) - 1) /
			sizeof(ULONG64)];

		PSYMBOL_INFO psi=(PSYMBOL_INFO)buffer;
		psi->SizeOfStruct = sizeof(SYMBOL_INFO);
		psi->MaxNameLen = MAX_SYM_NAME;
		if(SymFromAddr(hProcess,sf.AddrPC.Offset,NULL,psi))
		{
			char szFunName[MAX_PATH]={0};
#if defined(_WIN32)||defined(_WIN64)
			_snprintf_s(szFunName,MAX_PATH,MAX_PATH-1,"%s",psi->Name);
#else
			snprintf(szFunName,MAX_PATH-1,"%s",psi->Name)
#endif
			strStackLine+=szFunName;

		}


		//获取源文件和行号
		IMAGEHLP_LINE64 LineInfo;
		LineInfo.SizeOfStruct = sizeof(IMAGEHLP_LINE);
		DWORD dwLineDisplacement=0;
		if (SymGetLineFromAddr64(hProcess, sf.AddrPC.Offset, &dwLineDisplacement, &LineInfo))
		{
			char szFileLine[MAX_PATH]={0};
#if defined(_WIN32)||defined(_WIN64)
			_snprintf_s(szFileLine,MAX_PATH,MAX_PATH-1," %s.(%u)",LineInfo.FileName,LineInfo.LineNumber);
#else
			snprintf(szFileLine,MAX_PATH-1," %s.(%u)",LineInfo.FileName,LineInfo.LineNumber);
#endif
			strStackLine+=szFileLine;
		}
		else
		{
			strStackLine+=" No source file.";
		}

		strStackInfo+=strStackLine;
		strStackInfo+="\r\n";
	}


	SymCleanup(hProcess);

	return strStackInfo;
}




#endif
