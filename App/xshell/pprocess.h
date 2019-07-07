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

	//����˫���̱���ģʽ
	bool Start();

	//ֹͣ˫���̱���ģʽ
	void Stop();


private:

	//�����߳�
	void Work();

	//�������̼乲���¼���������
	static bool CreateEventName(std::string& event_name);

	//��ȡ��ִ��ģ��Ͳ���
	bool GetExeModuleName(std::string& exe_file);
private:
	ThreadPtr work_;
	bool active_;
	HANDLE event_;		//���̹����¼�����,����֪ͨ���������˳�
	std::string event_name_;	//���̼乲���¼���������
	std::string exe_name_;	//��ִ��ģ������
};


typedef std::shared_ptr<PProcess> PProcessPtr;



