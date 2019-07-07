
#include "pprocess.h"
#include <objbase.h>

PProcess::PProcess(void)
{
	active_=false;
	event_=NULL;
}

PProcess::~PProcess(void)
{
	Stop();
}


//启动双进程保护模式
bool PProcess::Start()
{
	
	//生成事件名称
	if(!CreateEventName(event_name_))
		return false;

	//创建事件对象
	event_=CreateEventA(NULL,TRUE,FALSE,event_name_.c_str());
	if(!event_)
		return false;

	//生成可执行模块
	if(!GetExeModuleName(exe_name_))
	{
		CloseHandle(event_);
		event_=NULL;
		return false;
	}

	active_=true;
	try
	{
		work_ = ThreadPtr(new std::thread([=]
		{
			Work();
		}));
		work_->detach();
	}
	catch(...)
	{
		CloseHandle(event_);
		event_=NULL;
		return false;
	}

	return true;
}

//停止双进程保护模式
void PProcess::Stop()
{
	active_=false;
	if(event_)
	{
		SetEvent(event_);
		CloseHandle(event_);
		event_=NULL;
	}
	if(work_)
	{
		work_->join();
		work_.reset();
	}

}

//工作线程
void PProcess::Work()
{
	int i=5;

	while(active_)
	{

		if(i>5)
		{
			i=0;

			STARTUPINFO si={0};
			si.cb=sizeof(si);
			PROCESS_INFORMATION pi={0};
			if(!CreateProcessA(NULL,(char*)exe_name_.c_str(),NULL,NULL,FALSE,CREATE_NO_WINDOW|NORMAL_PRIORITY_CLASS,NULL,NULL,(LPSTARTUPINFOA)&si,&pi))
			{
				printf("Can not create process \"%s\" error=%u\n",exe_name_.c_str(),GetLastError());
				continue;
			}

			WaitForSingleObject(pi.hProcess,INFINITE);

			CloseHandle(pi.hThread);
			CloseHandle(pi.hProcess);
			printf("Work process terminated.\n");
		}
		Sleep(1000);
		++i;
	}
}

//创建进程间共享事件对象名称
bool PProcess::CreateEventName(std::string& event_name)
{
	GUID g={0};
	HRESULT r=CoCreateGuid(&g);
	if(FAILED(r))
	{
		return false;
	}

	char szId[64]={0};
	_snprintf(szId,63,"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",g.Data1,g.Data2,g.Data3,
		g.Data4[0],g.Data4[1],g.Data4[2],g.Data4[3],g.Data4[4],g.Data4[5],g.Data4[6],g.Data4[7]);


	event_name="xshell_";
	event_name+=szId;
	return true;
}

//获取可执行模块和参数
bool PProcess::GetExeModuleName(string& exe_file)
{
	char cExeFile[MAX_PATH]={0};
	if(!GetModuleFileNameA(NULL,cExeFile,MAX_PATH))
	{
		return false;
	}
	

	exe_file="\"";
	exe_file+=cExeFile;
	exe_file+="\"";
	exe_file+=" -w ";
	exe_file+=" -v ";
	exe_file+=event_name_;

	return true;
}

