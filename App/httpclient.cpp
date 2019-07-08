#include "stdafx.h"
#include "httpclient.h"
#include "Logger.h"
#include "utils.h"
#include "desktopCapture.h"
bool CHttpClient::HandleHead(char* pdata, int len)
{
	char* pStart = (char*)pdata + m_sHttpType.length() + 1;
	std::string fullUrl = g_strtok(pStart, " HTTP");
	if (fullUrl == "")
	{
		return false;
	}
	m_strHttpPath = fullUrl;
	char*  sParam = strstr((char*)fullUrl.c_str(), "?");
	if (sParam != NULL && *(sParam + 1) != '\0')
	{
		sParam++;
		m_strHttpParam = sParam;
		m_strHttpPath = g_strtok((char*)fullUrl.c_str(), "?");
	}
	if (m_strHttpPath == "")
	{
		return false;
	}
	return true;

}
void CHttpClient::OnConnected(SOCKET  NewConnection)
{
	m_s = NewConnection;
	bool bStart = false;
	std::thread thread([=]
	{
		std::shared_ptr<CHttpClient> clent = shared_from_this();
		char* recvbuf = new char[1024 * 1024];

		// 也可以在这里调用bind函数绑定一个本地地址   
		// 否则系统将会自动安排   
		int len = 0;
		while (!m_bStop)
		{
			while (1)
			{
				Sleep(1);
				// 接收数据   
				while (1)
				{
					if (!Recv_i(recvbuf + len, 1))
					{
						break;
					}
					len++;
					int nEnd = *(int*)(recvbuf + len - 4);
					if (nEnd == 0x0A0D0A0D)
					{
						char* op;
						op = strtok(recvbuf, (" \r\n"));
						recvbuf[len] = 0;
						m_sHttpType = op;
						if (!HandleHead(recvbuf,len))
						{
							break;
						}
						
						break;

					}
				}
				if (m_sHttpType == "GET")
				{
					OnMessage(recvbuf, len);
					len = 0;
				}
				else
				{
					if (!Recv_i(recvbuf, 1))
					{
						break;
					}
				}
				
				
			}
		}
		// 关闭套节字   
		CloseSocket();
		delete[] recvbuf;
	});
	thread.detach();
	std::thread thread1([=, &bStart]
	{
		std::shared_ptr<CHttpClient> clent = shared_from_this();
		bStart = true;
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
	while (!bStart)
	{
		Sleep(1);
	}
}
void CHttpClient::OnDisconnect()
{

}
void CHttpClient::Send(std::string str)
{
	m_mutex.lock();
	m_SendMsg.push_back((str));
	m_mutex.unlock();
}
unsigned int  CHttpClient::ResquestHttpHeadAndData(  unsigned int httpCode, const std::string sHttpData, bool bIsNeedConvertUtf8)
{
	std::string strHttpSendData = sHttpData;
	if (bIsNeedConvertUtf8)
	{
		strHttpSendData = GBKToUTF8(strHttpSendData);
	}
	char httpHead[1024];
	std::string strhttpHead;
	std::string httpCodeString = GetCodeString(httpCode);
	sprintf(httpHead, ("HTTP/1.1 %d %s\r\n"), httpCode, httpCodeString.c_str());
	strhttpHead = httpHead;

	sprintf(httpHead, ("Server: DWK/1.1\r\n"));
	strhttpHead += httpHead;

	sprintf(httpHead, ("Pragma: no-cache\r\n"));
	strhttpHead += httpHead;

	sprintf(httpHead, ("Cache-control: no-cache\r\n"));
	strhttpHead += httpHead;

	sprintf(httpHead, ("Content-Length: %d\r\n"), strHttpSendData.length());
	strhttpHead += httpHead;

	std::string sContentType = "text/html; charset=utf-8";
	sprintf(httpHead, ("Content-Type: %s\r\n"), sContentType.c_str());
	strhttpHead += httpHead;

	sprintf(httpHead, ("\r\n"));
	strhttpHead += httpHead;

	strhttpHead += strHttpSendData;

	Send(strhttpHead);
	return strhttpHead.length();
}
void CHttpClient::OnMessage(char* pdata, int datalen)
{
	std::map < std::string, std::string> params;
	if (m_strHttpPath == "/query")
	{	
		httpParamToMap(m_strHttpParam, params);
		if (m_strHttpParam == "")
		{
			std::string str;
			for (int i = 1;i<=8;i++)
			{
				str  += GetDesktopCapture()->GetTableInfo(i);
			}
			ResquestHttpHeadAndData(200, str, true);
		}
		else
		{
			int tableId = atoi(params["tableid"].c_str());
			if (tableId > 0 &&tableId<10)
			{
				std::string str = GetDesktopCapture()->GetTableInfo(tableId);
				ResquestHttpHeadAndData(200, str, false);
			}		
		}
		
	}
}
bool CHttpClient::Send_i(char* buf, int len)
{
	//LOG_INFO("%s", buf);
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
bool CHttpClient::Recv_i(char* buf, int len)
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
void CHttpClient::CloseSocket()
{
	m_mutex.lock();
	if (m_s != INVALID_SOCKET)
	{
		::closesocket(m_s);
		m_s = INVALID_SOCKET;
		m_bConncet = false;
		m_SendMsg.clear();
		m_bStop = true;
	}
	m_mutex.unlock();

}