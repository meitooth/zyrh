#include <Windows.h>
#include <sstream>
#include "win_service.h"

WinService* WinService::s_ws_=NULL;

WinService::WinService(void)
{
	status_handle_=NULL;
}

WinService::~WinService(void)
{
}

WinService* WinService::GetInstance()
{
	if(!s_ws_)
		s_ws_=new WinService();

	return s_ws_;
}

void WinService::Release()
{
	if(s_ws_)
	{
		delete s_ws_;
		s_ws_=NULL;
	}
}

//安装服务
bool WinService::Install(const char* szDescr)
{
	SC_HANDLE sc=::OpenSCManagerA(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(!sc)
	{
		return false;
	}

	char szExeFile[MAX_PATH]={0};
	GetModuleFileNameA(NULL,szExeFile,MAX_PATH);
	stringstream ss;


	ss<<"\""<<szExeFile<<"\"";
	ss<<" -s";


	SC_HANDLE sc_svc=CreateServiceA(sc,service_name_.c_str(),service_name_.c_str(),SC_MANAGER_ALL_ACCESS,SERVICE_WIN32_OWN_PROCESS,SERVICE_AUTO_START,SERVICE_ERROR_NORMAL,ss.str().c_str(),NULL,NULL,NULL,NULL,NULL);
	if(!sc_svc)
	{
		::CloseServiceHandle(sc);
		return false;
	}

	if(szDescr)
	{
		SERVICE_DESCRIPTIONA descr={0};
		descr.lpDescription=(char*)(szDescr);
		ChangeServiceConfig2(sc_svc,SERVICE_CONFIG_DESCRIPTION,(void*)&descr);
	}

	::CloseServiceHandle(sc_svc);
	::CloseServiceHandle(sc);

	return true;
}

//卸载服务
bool WinService::Uninstall()
{
	SC_HANDLE sc=::OpenSCManagerA(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(!sc)
	{
		return false;
	}

	SC_HANDLE sc_svc=OpenServiceA(sc, service_name_.c_str(), SERVICE_ALL_ACCESS);
	if(!sc_svc)
	{
		CloseServiceHandle(sc);
		return false;
	}

	ControlService(sc_svc, SERVICE_CONTROL_STOP, NULL);

	if(!::DeleteService(sc_svc))
	{
		CloseServiceHandle(sc_svc);
		CloseServiceHandle(sc);
		return false;
	}

	CloseServiceHandle(sc_svc);
	CloseServiceHandle(sc);
	return true;
}

//启动服务
bool WinService::Run()
{
	SERVICE_TABLE_ENTRYA ste[]={{(LPTSTR)service_name_.c_str(),WinServiceMain},{NULL,NULL}};
	if(!StartServiceCtrlDispatcherA(ste))
	{
		return false;
	}

	return true;
}



void WINAPI WinService::WinServiceMain(DWORD dwNumServicesArgs,LPSTR *lpServiceArgVectors)
{
	WinService* s=GetInstance();
	s->status_handle_=RegisterServiceCtrlHandlerExA(s->service_name_.c_str(),WinServiceHandleEx,NULL);
	if(!s->status_handle_)
	{
		TerminateProcess(GetCurrentProcess(),-1);
		return;
	}

	s->SetStatus(SERVICE_START_PENDING,0);

	if(!s->RunStart())
	{
		TerminateProcess(GetCurrentProcess(),-1);
		return;
	}

	s->SetStatus(SERVICE_RUNNING,SERVICE_ACCEPT_STOP);
}

DWORD WINAPI WinService::WinServiceHandleEx(DWORD dwControl,DWORD dwEventType,LPVOID lpEventData,LPVOID lpContext)
{
	WinService* s=GetInstance();

	switch(dwControl)
	{
	case SERVICE_CONTROL_STOP:
		{
			s->SetStatus(SERVICE_START_PENDING,0);

			s->RunStop();

			s->SetStatus(SERVICE_ACCEPT_STOP,SERVICE_STOPPED);
			break;
		}
		//case SERVICE_CONTROL_PAUSE:
		//	{
		//	}
		//case SERVICE_CONTROL_CONTINUE:
		//	{
		//	}
	}

	return 0;
}

bool WinService::SetStatus(DWORD dwStatus,DWORD dwAccepted)
{
	SERVICE_STATUS status={SERVICE_WIN32_OWN_PROCESS};
	status.dwCurrentState=dwStatus;
	status.dwControlsAccepted=dwAccepted;
	if(!SetServiceStatus(status_handle_,&status))
	{
		return false;
	}

	return true;
}


//程序进入入口
bool WinService::RunStart()
{
	return pprocess_.Start();
}

//程序退出入口
void WinService::RunStop()
{
	pprocess_.Stop();
}



