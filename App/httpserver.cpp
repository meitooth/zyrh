#include "stdafx.h"
#include "httpserver.h"
#include "Logger.h"
#include "utils.h"
int CHttpServer::Run(int Port)
{
	m_nPort = Port;
	std::thread runthread([=]
	{
		WORD	wVersionRequested;
		WSADATA wsaData;
		int		err, iLen;
		wVersionRequested = MAKEWORD(2, 2);//create 16bit data
										   //(1)Load WinSock
		err = WSAStartup(wVersionRequested, &wsaData);	//load win socket
		if (err != 0)
		{
			return -1;
		}
		//(2)create socket
		SOCKET sockSrv = socket(AF_INET, SOCK_STREAM, 0);
		if (sockSrv == INVALID_SOCKET) {
			return -2;
		}
		//(3)server IP
		SOCKADDR_IN addrSrv;
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_addr.s_addr = htonl(INADDR_ANY);//Auto IP, byte sequence change
		addrSrv.sin_port = htons(m_nPort);
		//(4)bind
		err = bind(sockSrv, (SOCKADDR*)&addrSrv, sizeof(SOCKADDR));
		if (err != 0)
		{

			return -3;
		}
		//
		//(5)listen
		err = listen(sockSrv, 5);
		if (err != 0)
		{

			return -4;
		}
		//(6)client ip
		SOCKADDR_IN addrClt;
		int len = sizeof(SOCKADDR);
		while (1)
		{
			SOCKET NewConnection = accept(sockSrv, (SOCKADDR*)&addrClt, &len);
			
			if (NewConnection != INVALID_SOCKET)
			{
				std::shared_ptr<CHttpClient> pclient(new CHttpClient);
				pclient->OnConnected(NewConnection);
			}		
			else
			{
				int sockerr = WSAGetLastError();
				LOG_INFO("accept error %d %s ", sockerr, strerror(sockerr));
			}
			Sleep(10);
		}
		
		return 0;
	});
	runthread.detach();
	return 1;
	
}
void CHttpServer::Stop()
{
	closesocket(ListeningSocket);
}