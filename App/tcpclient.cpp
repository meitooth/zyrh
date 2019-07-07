#include "stdafx.h"
#include "tcpclient.h"
#include <mmiscapi2.h>
#include "Logger.h"
#include "utils.h"
#include <winsock.h>
#include <WS2tcpip.h>
void WINAPI onTimeKeepAlive(UINT uTimerID, UINT uMsg, DWORD_PTR dwUser, DWORD_PTR dw1, DWORD_PTR dw2)
{
	CTcpClient* pclient = (CTcpClient*)dwUser;
	pclient->OnTime();
}
CTcpClient::CTcpClient()
{
	if (m_TimerId == -1)
	{
		m_TimerId = timeSetEvent(30000, 1, (LPTIMECALLBACK)onTimeKeepAlive, (DWORD_PTR)this, TIME_PERIODIC);
	}
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
}
void CTcpClient::OnTime()
{
	if (m_clientEvent&&m_bConncet)
	{
		m_clientEvent->OnTimer();
	}
}
CTcpClient::~CTcpClient()
{
	timeKillEvent(m_TimerId);
	Stop();
}
void CTcpClient::CloseSocket()
{
	m_mutex.lock();
	if (m_s != INVALID_SOCKET)
	{
		::closesocket(m_s);
		m_s = INVALID_SOCKET;
		m_bConncet = false;
		m_SendMsg.clear();
	}
	m_mutex.unlock();

}
void CTcpClient::Stop()
{
	m_bStop = true;
	CloseSocket();
}
void CTcpClient::Close()
{
	CloseSocket();
}
bool CTcpClient::Send_i(char* buf, int len)
{
	//LOG_INFO("%s", buf);
	while (len > 0)
	{
		int sendlen = send(m_s, buf, len, 0);
		if (sendlen < 0)
		{
			int sockerr = WSAGetLastError();
			LOG_INFO("send error %d %s ", sockerr, strerror(sockerr));
			if (m_clientEvent)
			{
				m_clientEvent->OnDisconnect();
			}
			return false;
		}
		buf += sendlen;
		len -= sendlen;
	}
	return true;
}
bool CTcpClient::Recv_i(char* buf, int len)
{
	int readlen = len;
	int nTotalRecv = 0;
	while (len > 0)
	{
		int nRecvLen = ::recv(m_s, buf + nTotalRecv, len  , 0);
		if (nRecvLen < 0)
		{
			int sockerr = WSAGetLastError();
			if (sockerr == EINTR  )
				continue;
			if (m_clientEvent)
			{
				m_clientEvent->OnDisconnect();
			}			
			LOG_INFO("recv error %d %s ", sockerr, strerror(sockerr));
			CloseSocket();
			return false;
		}
		nTotalRecv += nRecvLen;
		len -= nRecvLen;
	}
	return true;

}
void CTcpClient::Connect(std::string ip, int nPort, ClientEvent* pclientEvent,bool bUseProxy, std::string proxyIp , int nProxyPort)
{
	m_ip = ip;
	m_nPort = nPort;
	m_clientEvent = pclientEvent;
	m_bUseProxy = bUseProxy;
	m_ProxyIp = proxyIp;
	m_nProxyPort = nProxyPort;
	
	std::thread thread([=]
	{
		char* recvbuf = new char[1024*1024];
		
		// 也可以在这里调用bind函数绑定一个本地地址   
		// 否则系统将会自动安排   

		// 填写远程地址信息   
		sockaddr_in servAddr;
		servAddr.sin_family = AF_INET;
		if (m_bUseProxy)
		{
			servAddr.sin_port = htons(m_nProxyPort);
		}
		else
		{
			servAddr.sin_port = htons(m_nPort);
		}
		
		// 注意，这里要填写服务器程序（TCPServer程序）所在机器的IP地址   
		// 如果你的计算机没有联网，直接使用127.0.0.1即可   
		
		while (!m_bStop)
		{
			std::string ip;
			if (m_bUseProxy)
			{
				ip = smtpIp((char*)m_ProxyIp.c_str());
			}
			else
			{
				ip = smtpIp((char*)m_ip.c_str());
			}
			servAddr.sin_addr.S_un.S_addr = inet_addr(ip.c_str());
			m_s = ::socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
			if (m_s == INVALID_SOCKET)
			{
				printf(" Failed socket() \n");
				return;
			}
			struct timeval tv = { 10,0 };

			//setsockopt(m_s, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(tv));
			if (::connect(m_s, (sockaddr*)&servAddr, sizeof(servAddr)) == -1)
			{
				LOG_INFO("connect failed %s:%d", ip.c_str(), m_nPort);
				Sleep(1000);
				m_bConncet = false;
				continue;
			}
			else
			{
				LOG_INFO("connect success %s:%d", ip.c_str(), m_nPort);
				if (m_bUseProxy)
				{
					check_up_socket_5(m_s, "1234", "1234", (char*)m_ip.c_str(), m_nPort);
				}
				if (m_clientEvent)
				{
					m_clientEvent->OnConnected();
				}
				m_bConncet = true;
			}
			while (1)
			{
				Sleep(1);
				// 接收数据   	
				if (!Recv_i(recvbuf, 2))
				{
					break;
				}
				if (!Recv_i(recvbuf, 6))
				{
					break;
				}
				char buflen[7];
				buflen[6] = 0;
				memcpy(buflen, recvbuf, 6);
				int len = atoi(buflen);

				if (!Recv_i(recvbuf, len + 6))
				{
					break;
				}
				recvbuf[len] = 0;
			
				LOG_INFO("%s %d ", recvbuf,len);
				m_clientEvent->OnMessage(recvbuf, len);
			}
			Sleep(3000);
		}
		

		

		// 关闭套节字   
		CloseSocket();
		delete[] recvbuf;
	});
	thread.detach();
	std::thread thread1([=]
	{
		while (!m_bStop)
		{
			Sleep(1);
			if (m_s != INVALID_SOCKET)
			{
				std::vector<std::string> msglist;
				m_mutex.lock();
				msglist = m_SendMsg;
				m_SendMsg.clear();
				m_mutex.unlock();

				for (int i = 0;i < msglist.size();i++)
				{
					 if (!Send_i((char*)msglist[i].c_str(), msglist[i].length()))
					 {
						 break;
					 }
				}
			}
			
			

		}
	});
	thread1.detach();
}
void CTcpClient::Send(std::string str)
{
	return;
	LOG_INFO("%s", str.c_str());
	m_mutex.lock();
	m_SendMsg.push_back(encoderMsg(str));
	m_mutex.unlock();
}
#define SOCKS_VER		    0x05

#define METHOD_AUTH_NO		0x00
#define METHOD_AUTH			0x02
#define METHOD_GSSAPI		0x01
#define METHOD_NO_ACCEPT	0xFF

#define SOCKS_AUTH_VER      0x01
#define SOCKS_AUTH_OK       0x00

#define CMD_CONNECT			0x01
#define CMD_BIND			0x02
#define CMD_UDP				0x03

#define FIELD_RSV			0x00

#define ATYP_IPV4			0x01
#define ATYP_DOMAINNAME		0x03
#define ATYP_IPV6			0x04

#define REP_SUCCESS			0x00
#define REP_FAIL_GENERAL	0x01
#define REP_CONNECT_DENY	0x02
#define REP_UNREACHABLE		0x03
#define REP_HOST_NOTFIND	0x04
#define REP_REFUSED			0x05
#define	REP_TTL_EXPIRED		0x06
#define REP_NOT_SUPPORT		0x07
#define REP_ATYP_NOTSUPPORT 0x08

typedef struct sock5req1
{
	char Ver;
	char nMethods;
	char Methods[255];
}sock5req1;

typedef struct sock5ans1
{
	char Ver;
	char Method;
}sock5ans1;

typedef struct sock5req2
{
	char Ver;
	char Cmd;
	char Rsv;
	char Atyp;
	char other[1];
}sock5req2;

typedef struct sock5ans2
{
	char Ver;
	char Rep;
	char Rsv;
	char Atyp;
	char other[1];
}sock5ans2;

typedef struct socks5authreq
{
	char Ver;
	char other[1];
}socks5authreq;

typedef struct socks5authans
{
	char Ver;
	char Status;
}socks5authans;
int CTcpClient::check_up_socket_5(int sClient, char* str_username, char* str_pwd, char* str_destaddr, int nDestPort)
{
	char buffer[1024];
	int bufferlen = 1023;
	int ret;
	sock5req1 *m_proxyreq1;
	sock5ans1 *m_proxyans1;
	char *DestAddr = str_destaddr;
	sock5ans2 *m_proxyans2;
	sock5req2 *m_proxyreq2;
	unsigned short pt = htons(nDestPort);
	int len = 0;
	struct in_addr goal;


	m_proxyreq1 = (sock5req1 *)buffer;
	m_proxyreq1->Ver = SOCKS_VER;
	m_proxyreq1->nMethods = 2;
	m_proxyreq1->Methods[0] = METHOD_AUTH_NO;
	m_proxyreq1->Methods[1] = METHOD_AUTH;

	ret = send(sClient, buffer, 4, 0);
	if (ret == -1)
	{
		//closesocket(sClient);
		return 0;
	}

	ret = recv(sClient, buffer, bufferlen, 0);
	if (ret == -1)
	{
		//closesocket(sClient);
		return 0;
	}


	m_proxyans1 = (sock5ans1 *)buffer;

	if (m_proxyans1->Ver == SOCKS_VER && m_proxyans1->Method == METHOD_AUTH_NO && m_proxyans1->Method == METHOD_AUTH)
	{
		//closesocket(sClient);
		return 0;
	}

	if (m_proxyans1->Method == METHOD_AUTH)
	{
		char *AuthName = str_username;
		char *AuthPasswd = str_pwd;
		socks5authreq *m_authreq = 0;
		int iNameLen = strlen(AuthName);
		int iPasswd = strlen(AuthPasswd);
		socks5authans *m_authans = 0;
		m_authreq = (socks5authreq *)buffer;

		m_authreq->Ver = SOCKS_AUTH_VER;
		m_authreq->other[0] = iNameLen;
		strcpy(m_authreq->other + 1, AuthName);
		m_authreq->other[iNameLen + 1] = iPasswd;
		strcpy(m_authreq->other + iNameLen + 2, AuthPasswd);

		ret = send(sClient, buffer, 3 + iNameLen + iPasswd, 0);
		if (ret == -1)
		{
			//closesocket(sClient);
			return 0;
		}

		ret = recv(sClient, buffer, bufferlen, 0);
		if (ret == -1)
		{
			printf("recv()");
			//closesocket(sClient);
			return 0;
		}


		m_authans = (socks5authans *)buffer;

		if (m_authans->Ver != SOCKS_AUTH_VER && m_authans->Status != SOCKS_AUTH_OK)
		{
			//printf("身份验证失败ERROR");
			//closesocket(sClient);
			return 0;
		}
	}



	m_proxyreq2 = (sock5req2 *)buffer;
	m_proxyreq2->Ver = SOCKS_VER;
	m_proxyreq2->Cmd = CMD_CONNECT;
	m_proxyreq2->Rsv = FIELD_RSV;
	goal.s_addr = inet_addr(DestAddr);
	if (goal.s_addr != INADDR_NONE)
	{
		m_proxyreq2->Atyp = ATYP_IPV4;
		memcpy(&m_proxyreq2->other[0], &goal, 4);
		memcpy(&m_proxyreq2->other[4], &pt, 2);
		len = 10;
	}
	else
	{
		m_proxyreq2->Atyp = ATYP_DOMAINNAME;
		len = strlen(DestAddr);
		m_proxyreq2->other[0] = len;
		memcpy(&m_proxyreq2->other[1], DestAddr, len);
		memcpy(&m_proxyreq2->other[1] + len, &pt, 2);
		len += 7;
	}

	ret = send(sClient, buffer, len, 0);
	if (ret == -1)
	{
		//closesocket(sClient);
		return 0;
	}

	ret = recv(sClient, buffer, bufferlen, 0);
	if (ret == -1)
	{
		//closesocket(sClient);
		return 0;
	}


	m_proxyans2 = (sock5ans2 *)buffer;

	if (m_proxyans2->Rep != REP_SUCCESS)
	{
		//closesocket(sClient);
		return 0;
	}
	return 1;

}
int CTcpClient::check_up_socket_4(int sClient, char* str_username, char* str_pwd, char* str_destaddr, int nDestPort)
{
	char command[1024];
	char commandret[9];
	int len = 9;
	unsigned short tmpPort = nDestPort;
	unsigned short uPort = htons(tmpPort);
	unsigned short uPort1 = htons(nDestPort);
	struct sockaddr_in service;
	char* hostname = str_destaddr;


	int ret = TRUE;
	int ret1 = 0;
	struct in_addr addr;
	memset(command, 0, len + 1);
	ret1 = inet_pton(AF_INET, str_destaddr, &addr);
	if (ret1 <= 0)
	{
		struct hostent *host = gethostbyname(str_destaddr);
		if (host == NULL || host->h_addr == NULL)
		{
			ret = 0;
		}

		service.sin_addr = *(struct in_addr *)host->h_addr;
		hostname = inet_ntoa(service.sin_addr);


	}
	else
	{
		service.sin_addr.s_addr = inet_addr(str_destaddr);

	}
	len += strlen(hostname);

	command[0] = 4;
	command[1] = 1; //CONNECT or BIND request

	memcpy(&command[2], &uPort, 2); //Copy target address
	if (!hostname)
	{
		//Set the IP to 0.0.0.x (x is nonzero)
		command[4] = 0;
		command[5] = 0;
		command[6] = 0;
		command[7] = 1;
		//Add host as URL
	}
	else
	{
		unsigned long uAddr = service.sin_addr.s_addr;
		memcpy(&command[4], &uAddr, 4);
	}
	strcpy(&command[8], hostname);
	if (send(sClient, command, len, 0) <= 0)
	{
		return 0;
	}
	len = recv(sClient, commandret, 8, 0);
	if (len == 8)
	{
		if (commandret[1] != 90 || commandret[0] != 0)
		{
			return 0;
		}
		return 1;
	}
	return 0;
}