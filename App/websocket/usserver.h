#pragma once
#include <string>
#include <mutex>
#include <map>
#include <vector>
#include "usclient.h"
class UWebSocketsServerEvent;
class  UWebSocketsServer :public std::enable_shared_from_this<UWebSocketsServer>
{
public:
	UWebSocketsServer();
	~UWebSocketsServer();
	void Start(int nPort = 3000);

private:
	int m_nPort = 0;


};

class UWebSocketsServerClient
{
public:
	UWebSocketsServerClient()
	{

	}
	~UWebSocketsServerClient()
	{

	}
	virtual void onConnection()
	{

	}
	virtual void onDisconnection()
	{
	}
	virtual void OnMessage(std::string message)
	{
	}
	virtual void SendData(std::string str);
public:
	void *m_ws = NULL;
};