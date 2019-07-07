#pragma  once
#include <thread>
#include <string>
#include <mutex>
#include <vector>
#include <winsock2.h>
#include "httpclient.h"
#include <map>
class CHttpServer
{
public:
	CHttpServer()
	{

	}
	~CHttpServer()
	{

	}
	int Run(int Port);
	void Stop();
private:
	int m_nPort = 0;
	std::map<void*, void*> m_clientList;
	std::mutex m_mutex;
	SOCKET     ListeningSocket = INVALID_SOCKET;

};