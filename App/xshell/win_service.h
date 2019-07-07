#pragma once

#include <string>
#include "pprocess.h"


class WinService
{
private:
	WinService(void);
	~WinService(void);

public:
	static WinService* GetInstance();
	static void Release();

	void SetServiceName(const char* service_name){
		service_name_=service_name;
	}

	//安装服务
	bool Install(const char* szDescr);

	//卸载服务
	bool Uninstall();

	//启动服务
	bool Run();
private:
	static void WINAPI WinServiceMain(DWORD dwNumServicesArgs,LPSTR *lpServiceArgVectors);
	static DWORD WINAPI WinServiceHandleEx(DWORD dwControl,DWORD dwEventType,LPVOID lpEventData,LPVOID lpContext);

	//设置服务状态
	bool SetStatus(DWORD dwStatus,DWORD dwAccepted);

	//程序进入入口
	bool RunStart();

	//程序退出入口
	void RunStop();
private:
	std::string service_name_;		//服务名称

	SERVICE_STATUS_HANDLE status_handle_;
	PProcess pprocess_;

	static WinService* s_ws_;
};






