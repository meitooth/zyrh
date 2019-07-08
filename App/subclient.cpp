#include "stdafx.h"
#include "subclient.h"
#include "desktopCapture.h"
#include "cmdMainSub.h"
#include "Logger.h"
#include "appclient.h"

void SubClient::OnMessage(char* pdata, int datalen)
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
	std::string strType = root["Type"].asCString();
	if (strType == "SubBetsRequest")
	{
		SubBetsRequest request;
		request.parse(json.c_str());
		if (request.TableId > 0)
		{
			//m_TableSubList[request.TableId - 1].SetBetInfo(request);
		}

	}
	else if (strType == "KeepLive")
	{
		m_lasttime = time(NULL);
	}
}
void SubClient::OnConnected()
{
	SubRegisterRequest request;
	request.DeviceId = GetProductID();
	request.SeqNum = m_Seq++;
	request.DeviceId = replace_all(request.DeviceId, "-", "");
	request.MachineCode = GetAppClient()->MachineCode;
	m_client.Send(request.ToStr());
	LOG_INFO("%s ", request.ToStr().c_str());
}
void SubClient::OnDisconnect()
{

}
void SubClient::OnTimer()
{
	KeepAliveRequest request;
	request.SeqNum = m_Seq++;
	m_client.Send(request.ToStr());
}
void SubClient::Connect(char* ip, int port)
{
	m_client.Connect(ip, port, this);
}
SubClient* GetSubClient()
{
	static SubClient* p = NULL;
	if (!p)
	{
		p = new SubClient;
	}
	return p;
}