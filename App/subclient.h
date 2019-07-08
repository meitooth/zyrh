#pragma once
#include "tcpclient.h"
class SubClient :public ClientEvent
{
public:
	SubClient()
	{

	}
	~SubClient()
	{

	}
	void Connect(char* ip, int port);
	void OnConnected();
	void OnDisconnect();
	void OnMessage(char* pdata, int datalen);
	void OnTimer();
private:
	int m_Seq = 0;
	CTcpClient m_client;
	int m_lasttime = NULL;
};
extern SubClient* GetSubClient();