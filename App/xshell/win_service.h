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

	//��װ����
	bool Install(const char* szDescr);

	//ж�ط���
	bool Uninstall();

	//��������
	bool Run();
private:
	static void WINAPI WinServiceMain(DWORD dwNumServicesArgs,LPSTR *lpServiceArgVectors);
	static DWORD WINAPI WinServiceHandleEx(DWORD dwControl,DWORD dwEventType,LPVOID lpEventData,LPVOID lpContext);

	//���÷���״̬
	bool SetStatus(DWORD dwStatus,DWORD dwAccepted);

	//����������
	bool RunStart();

	//�����˳����
	void RunStop();
private:
	std::string service_name_;		//��������

	SERVICE_STATUS_HANDLE status_handle_;
	PProcess pprocess_;

	static WinService* s_ws_;
};






