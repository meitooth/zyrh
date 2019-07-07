#include "stdafx.h"
#include "serverwork.h"
#include "Logger.h"
#include "utils.h"
#include "desktopCapture.h"
#include "cmdMainSub.h"
#include "appclient.h"
void CServerWork::OnConnected(SOCKET  NewConnection)
{
	m_s = NewConnection;
	bool threadstart = false;
	std::thread thread([=]
	{
		std::shared_ptr<CServerWork> ptr = shared_from_this();
		char* recvbuf = new char[1024 * 1024];

		// 也可以在这里调用bind函数绑定一个本地地址   
		// 否则系统将会自动安排   

		while (!m_bStop)
		{
			while (1)
			{
				Sleep(1);
				// 接收数据   	
				if (!Recv_i(recvbuf, 2))
				{
					break;
				}
				if (!Recv_i(recvbuf, 4))
				{
					break;
				}
				char buflen[5];
				buflen[4] = 0;
				memcpy(buflen, recvbuf, 4);
				int len = atoi(buflen);

				if (!Recv_i(recvbuf, len + 6))
				{
					break;
				}
				recvbuf[len] = 0;

				OnMessage(recvbuf, len);
			}
		}
		// 关闭套节字   
		CloseSocket();
		delete[] recvbuf;
	});
	
	thread.detach();
	std::thread thread1([=, &threadstart]
	{
		std::shared_ptr<CServerWork> ptr = shared_from_this();
		threadstart = true;
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
	while (!threadstart)
	{
		Sleep(1);
	}
	thread1.detach();
}
void CServerWork::OnDisconnect()
{
	GetAppClient()->nSubLastTIme = 0;
}
void CServerWork::Send(std::string str)
{
	m_mutex.lock();
	m_SendMsg.push_back(encoderMsg(str));
	m_mutex.unlock();
}
void CServerWork::OnMessage(char* pdata, int datalen)
{
	std::string json = decoderMsg(pdata, datalen);
	LOG_INFO("%s ", json.c_str());
	Json::Reader reader;
	Json::Value root;
	if (!reader.parse(json.c_str(), root, false))
	{
		return;
	}
	if (root["Type"].isNull())
	{
		return;
	}
	GetAppClient()->SubLastTimeOut = GetCurrentTime2Str();
	GetAppClient()->nSubLastTIme = time(NULL);
	std::string strType = root["Type"].asCString();
	if (strType == "SubBetsResponse")
	{
		SubBetsResponse response;
		response.parse(json.c_str());
		if (response.TableId > 0)
		{
			//m_TableList[response.TableId - 1].SetBetResult(response);
		}

	}
	else if (strType == "KeepLive")
	{
		KeepAliveRequest request;
		request.parse(json);
		m_keeplivetime = time(NULL);
		KeepAliveResponse resopnse;
		resopnse.SeqNum = request.SeqNum;
		Send(resopnse.ToStr());
	}
	else if (strType == "Register")
	{
		GetAppClient()->serverwork_wptr = shared_from_this();
		SubRegisterRequest request;
		request.Parse(json);
		
		SubRegisterResponse resopnse;
		resopnse.SeqNum = request.SeqNum;
		Send(resopnse.ToStr());
		GetAppClient()->SubMachineCode = request.MachineCode;
		GetAppClient()->SubDeviceId = request.DeviceId;
	}
}
bool CServerWork::Send_i(char* buf, int len)
{
	/*LOG_INFO("%s", buf);*/
	while (len > 0)
	{
		int sendlen = send(m_s, buf, len, 0);
		if (sendlen < 0)
		{
			int sockerr = WSAGetLastError();
			LOG_INFO("send error %d %s ", sockerr, strerror(sockerr));

			return false;
		}
		len -= sendlen;
	}
	return true;
}
bool CServerWork::Recv_i(char* buf, int len)
{
	int readlen = len;
	while (len > 0)
	{
		int nRecvLen = ::recv(m_s, buf, len, 0);
		if (nRecvLen < 0)
		{
			int sockerr = WSAGetLastError();
			if (sockerr == EINTR)
				continue;;
			LOG_INFO("recv error %d %s ", sockerr, strerror(sockerr));
			CloseSocket();
			return false;
		}
		len -= nRecvLen;
	}
	return true;

}
void CServerWork::CloseSocket()
{
	m_mutex.lock();
	if (m_s != INVALID_SOCKET)
	{
		::closesocket(m_s);
		m_s = INVALID_SOCKET;
		m_bConncet = false;
		m_SendMsg.clear();
		m_bStop = true;
		OnDisconnect();
	}
	m_mutex.unlock();

}