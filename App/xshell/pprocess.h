#pragma once
 
#include <thread>
#include <memory>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

using namespace std;

class PProcess
{
public:
	typedef std::shared_ptr<std::thread> ThreadPtr;
	PProcess(void);
	~PProcess(void);

	//启动双进程保护模式
	bool Start();

	//停止双进程保护模式
	void Stop();


private:

	//工作线程
	void Work();

	//创建进程间共享事件对象名称
	static bool CreateEventName(std::string& event_name);

	//获取可执行模块和参数
	bool GetExeModuleName(std::string& exe_file);
private:
	ThreadPtr work_;
	bool active_;
	HANDLE event_;		//进程共享事件对象,用于通知工作进程退出
	std::string event_name_;	//进程间共享事件对象名称
	std::string exe_name_;	//可执行模块名称
};


typedef std::shared_ptr<PProcess> PProcessPtr;



