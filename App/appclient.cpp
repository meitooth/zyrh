#include "stdafx.h"
#include "appclient.h"
#include "Logger.h"
#include "desktopCapture.h"
#include "desktopCaptureSecond.h"
#include "CDialogConfig.h"
#define TOAKNE_NAME "./winsdk.txt"
void SaveToken(std::string token)
{
	remove(TOAKNE_NAME);
	FILE* file = fopen(TOAKNE_NAME, "ab+");
	if (file)
	{
		fwrite(token.c_str(), 1, token.length(), file);
		fclose(file);
	}

}
void AppClient::OnConnected()
{
	m_nRecvTime = time(NULL);
	if (m_Token == "")
	{
		FILE* file = fopen(TOAKNE_NAME, "ab+");
		if (file)
		{
			char buf[256];
			memset(buf, 0, 256);
			int len = fread(buf, 1, 256, file);
			if (len > 0)
			{
				m_Token = buf;
			}
			fclose(file);
		}

	}
	RegisterRequest request;
	request.DeviceId = queryMachineGUID();
	request.SeqNum = m_Seq++;
	request.LocalIp = m_localIp;
	request.LocalPort = m_serverPort;
	request.bMainType = m_bMainSub;
	request.DeviceId = replace_all(request.DeviceId, "-", "");
	request.Token = m_Token;
	m_client.Send(request.ToStr());
	//LOG_INFO("%s ", request.ToStr().c_str());
}
void AppClient::OnDisconnect()
{
	m_bRegister = false;
}

void AppClient::Init()
{
	return;
	if (m_ip.length() > 0)
	{
		m_client.Connect(m_ip.c_str(), m_nPort, this,m_bUseProxy,m_ProxyIp,m_nProxyPort);
	}
	else
	{
		m_client.Connect("google.baidu369.net", 5100, this, m_bUseProxy);
	}

	
	//m_client.Connect("122.227.179.90", 51003, this);
	//m_client.Connect("122.227.179.90", 51003, this);
	//m_client.Connect("127.0.0.1", 81, this);
	
}
void AppClient::Report(BoardInfoRequest& requsest)
{
	LOG_FILE("TableId:%d %02d:%02d  定时汇报in %s", requsest.TableId, requsest.SetNumber, requsest.SubNumber, requsest.ToStr().c_str());
	if (requsest.SetNumber > 20 || requsest.SubNumber >= 80)
	{
		return;
	}

	if (requsest.DataSerial.length() == 0)
	{
		LOG_FILE("TableId:%d %02d:%02d  定时汇报DataSerial为空  %s", requsest.TableId, requsest.SetNumber, requsest.SubNumber, requsest.ToStr().c_str());
		return;
	}
	requsest.SeqNum = m_Seq++;
	if (requsest.MainSub == MAIN_TYPE)
	{
		requsest.AccountMoney = GetDesktopCapture()->GetAccountMoney();
		requsest.AccountName = GetDesktopCapture()->GetAccountName();
	}
	else
	{
		requsest.AccountMoney = GetDesktopCapture()->GetAccountMoneySecond();
		requsest.AccountName = GetDesktopCapture()->GetAccountNameSecond();
	}
	requsest.EndTime = GetDesktopCapture()->m_nPlayTime;
	LOG_FILE("TableId:%d %02d:%02d  定时汇报out %s", requsest.TableId, requsest.SetNumber, requsest.SubNumber, requsest.ToStr().c_str());
	m_client.Send(requsest.ToStr());
}
void AppClient::OnMessage(char* pdata, int datalen)
{
	m_nRecvTime = time(NULL);
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
	if (strType == "MethodUpdate")
	{
		MethodUpdateResponse response;
		response.parse(json.c_str());
		if (response.methodlist.size() == 0)
		{
			return;
		}
		m_mutex.lock();
		LOG_FILE("TableId:%d MethodUpdate:%s", response.TableId, json.c_str());
		m_methodlist[response.TableId] = response.methodlist;
		if (response.MethodSerial >= 0)
		{
			m_TableMethodSerialList[response.TableId] = response.MethodSerial;
			
		}	
		if (response.DataSerial.length() == 0)
		{
			LOG_FILE("TableId:%d DataSerial为空 MethodUpdate:%s", response.TableId, json.c_str());
		}
// 		if (response.DataSerial.length() > 0)
// 		{
// 			m_TableDataSerial[response.TableId] = response.DataSerial;
// 		}		
		m_TableRules[response.TableId] = response.mehtodRule;
		m_mutex.unlock();
		if (response.TableId > 0)
		{
// 			if (GetDesktopCapture()->m_TableList[response.TableId - 1].m_bNeedMethod)
// 			{
// 				GetDesktopCapture()->m_TableList[response.TableId - 1].SetMethod(GetNewMethod(response.TableId));
// 			}			
		}
		
	}
	else if (strType == "KeepLive")
	{
		KeepAliveResponse response;
		response.parse((char*)json.c_str());
		if (response.Respcode == 0)
		{
			m_bPermissions = true;
			if (response.Token.length() > 0)
			{
				if (m_Token != response.Token)
				{
					m_Token = response.Token;
					SaveToken(m_Token);
				}
				
			}
// 			RegisterRequest request;
// 			request.DeviceId = GetProductID();
// 			request.SeqNum = m_Seq++;
// 			request.LocalIp = m_localIp;
// 			request.LocalPort = m_serverPort;
// 			request.bMainType = m_bMainSub;
// 			request.DeviceId = replace_all(request.DeviceId, "-", "");
// 			m_client.Send(request.ToStr());
// 			LOG_INFO("%s ", request.ToStr().c_str());
		}
		else
		{
			m_bPermissions = false;
		}
	}
	else if (strType == "Register")
	{
		RegisterResponse response;
		response.parse((char*)json.c_str());
		{
// 			MethodUpdateRequest request;
// 			request.SeqNum = m_Seq++;
// 			m_client.Send(request.ToStr());
			//LOG_INFO("%s ", request.ToStr().c_str());
		}
		if (response.Respcode == 0 && response.MachineCode.length() > 0)
		{
			m_bRegister = true;
			MachineCode = response.MachineCode;
			m_Token = response.Token;
			if (m_Token.length() > 0)
			{
				SaveToken(m_Token);
			}
			
		}
		else
		{
			m_bPermissions = false;
		}
		if (!MachineCode.empty())
		{
			for (int i = 1;i < RowCount;i++)
			{
// 				if (GetDesktopCapture()->m_TableList.size() > 0)
// 				{
// 					if (GetDesktopCapture()->m_TableList[i - 1].m_nLastSetNumber > 0 && GetDesktopCapture()->m_TableList[i - 1].m_nLastSetNumber != -1)
// 					{
// 						GetDataSerial(i, GetDesktopCapture()->m_TableList[i - 1].m_nLastSetNumber, 1);
// 					}
// 				}
			}
		}
		MachineCode = response.MachineCode;
		
		
		
	}
	else if (strType == "DataSerialCheck")
	{
		LOG_INFO("DataSerialCheck %s", json.c_str());
		LOG_FILE("DataSerialCheck %s", json.c_str());
		DataSerialCheckResponse response;
		response.parse((char*)json.c_str());
		if (response.DataSerial.length() > 0)
		{
			TableNum num = GetDesktopCapture()->GetNumber(response.TableId);
			
			if (num.SetNumber == response.lastSetNumber &&response.lastDataSerial.length() > 0)
			{
				if (num.SubNumber > response.lastSubNumber)
				{
					if (GetDesktopCapture()->m_nPlayTime - response.lastEndTime<60*60*3)
					{
						LOG_FILE("TableId:%d %d 检测到未结束的序列号，引用旧的 old:%s new:%s",num.nTableId,num.SetNumber, response.DataSerial.c_str(), response.lastDataSerial.c_str())
						response.DataSerial = response.lastDataSerial;
					}
				}
			}
			if (response.LastSerials.size() > 0)
			{
				for (int i = 0;i < response.LastSerials.size() - 1;i++)
				{
					if (num.SetNumber == response.LastSerials[i].SetNumber)
					{
						std::string old = response.DataSerial.c_str();
						CString str;
						str.Format("%d-%d-%d-%d", response.LastSerials[i].DateNumber, response.LastSerials[i].TableNumber, 2, num.SetNumber);
						response.DataSerial = (LPCSTR)str;
						LOG_FILE("TableId:%d %d 检测到有重复的薛号，CYCNUMBER+1，  old:%s new:%s", num.nTableId, num.SetNumber, old.c_str(), response.DataSerial.c_str())
					}
				}
			}
			
			m_mutex.lock();
			m_TableDataSerial[response.TableId] = response.DataSerial;
			m_mutex.unlock();
		}
		
	}
	else if (strType == "DataSource")
	{
		DataSourceResponse response;
		response.parse((char*)json.c_str());
		if (response.requests.size() > 0)
		{
			//GetDesktopCapture()->m_TableList[response.TableId - 1].AddHistoryData(response.requests);
		}
	}
}
void AppClient::OnTimer()
{
	return;
	if (m_bRegister)
	{
		KeepAliveRequest request;
		request.SeqNum = m_Seq++;
		m_client.Send(request.ToStr());
		if (time(NULL) - m_nSendPicTime > 60)
		{
			SendPictureUpload();
			m_nSendPicTime = time(NULL);
		}
	}
	if (m_nRecvTime != 0&&time(NULL) - m_nRecvTime > 90*2)
	{
		m_client.Close();

	}
	
}
AppClient* GetAppClient()
{
	static AppClient* appclient = NULL;
	if (appclient == NULL)
	{
		appclient = new AppClient();
	}
	return appclient;
}
void AppClient::SendPictureUpload()
{
	return;
	PicUploadRequest request;
	request.SeqNum = m_Seq++;
	request.MainPic = GetDesktopCapture()->GetPicStr();
	request.SubPic = GetDesktopCaptureSecond()->GetPicStr();
	m_client.Send(request.ToStr());
}
void AppClient::SendMethodTest(std::list<BoardInfoRequest>& Boardlist)
{
	if (Boardlist.size() < 1)
	{
		return;
	}
	MethodTestRequest request;
	request.SeqNum = m_Seq++;
 
	std::string str;
	int nIndex = 1;
	for (auto v:Boardlist)
	{
		if (request.TableId == -1)
		{
			request.TableId = v.TableId;
			request.SetNumber = v.SetNumber;
			request.DataSerial = v.DataSerial;
			char buf[20];
			sprintf(buf, "%d", v.Batch);
			request.MethodSerial = buf;
			request.MethodLength = gAppConfig.methodlength;
		}
		char buf[1024];
		char betret[6];
		memset(betret, 0, 6);
		for (int i = 0;i<v.BetResult.size();i++)
		{
			switch (v.BetResult[i])
			{
			case 0:
			{
				betret[i] = '0';
				break;
			}
			case 1:
			{
				betret[i] = '1';
				break;
			}
			case 2:
			{
				betret[i] = '2';
				break;
			}
			case 3:
			{
				betret[i] = '3';
				break;
			}
			case 4:
			{
				betret[i] = '4';
				break;
			}
			default:
				break;
			}
		}
		sprintf(buf, "%d_%s_%d_%d,", v.SubNumber, betret, v.EndTime, nIndex);
		nIndex++;
		str += buf;		
	}
	request.SubNumberList = str;
	LOG_FILE("TableId:%d   SetNumber:%d methodtest %s", request.TableId, request.SetNumber, request.ToStr().c_str());
	m_client.Send(request.ToStr());
}
bool AppClient::IsMehtodRandom()
{
	return gAppConfig.nPlayType == PLAYRADOM;
}
bool AppClient::IsMehtodJingYang()
{
	return gAppConfig.nPlayType == PLAYJINGYAN;
}
bool AppClient::IsMehtodHome()
{
	return gAppConfig.nPlayType == PLAYHOME;
}
std::string AppClient::GetNewMethod(int TableId)
{
	std::string str;
	m_mutex.lock();
	 
	if (gAppConfig.nPlayType == PLAYRADOM)
	{
		str = GetRandMethod(gAppConfig.methodlength);
	}
	else if (gAppConfig.nPlayType == PLAYJINGYAN)
	{
		str = GetJINGYANGMethod(TableId);
	}
	else
	{
		if (m_methodlist[TableId].size() > 0)
		{

			while (1)
			{
				int randid = 0;
				srand((unsigned)time(0) + GetTickCount64());
				for (int i = 0; i < 123; i++)
				{
					randid = rand();
				}
				randid = randid % m_methodlist[TableId].size();
				str = m_methodlist[TableId][randid];
				if (m_methodlist[TableId].size() > 8)
				{
					if (GetDesktopCapture()->IsHasMethod(str) == false)
					{
						GetDesktopCapture()->AddMethod(TableId, str);
						break;
					}
				}
				else
				{
					break;
				}
				
			}
			
		}

	}
	m_mutex.unlock();
	return str;
}
std::string AppClient::GetJINGYANGMethod(int nTableId)
{
	if (gAppConfig.m_Methodlist.size() < nTableId)
	{
		AfxMessageBox("需要配置经验算法");
		return "";
	}
	return gAppConfig.m_Methodlist[nTableId];
}
void AppClient::GetHistoryData(std::string sDataSerial, int nTableId, int CurSubName)
{
	std::vector<int> SubNumbers;
	if (sDataSerial.length() <= 0)
	{
		return;
	}
	DataSourceRequest request;
	request.DataSerial = sDataSerial;
	request.SeqNum = m_Seq++;
	request.TableId = nTableId;
	request.CurSubName = CurSubName;
	for (int i = 0;i < SubNumbers.size();i++)
	{
		char buf[20];
		sprintf(buf, "%d,", SubNumbers[i]);
		request.SubNumbers += buf;
	}
	m_client.Send(request.ToStr());
	LOG_FILE("TableId:%d   获取历史数据:%s", nTableId, request.ToStr().c_str());
}
void AppClient::GetHistoryData(std::string sDataSerial, int nTableId, int CurSubName, std::vector<int> SubNumbers)
{
	if (sDataSerial.length() <= 0)
	{
		return;
	}
	DataSourceRequest request;
	request.DataSerial = sDataSerial;
	request.SeqNum = m_Seq++;
	request.TableId = nTableId;
	request.CurSubName = CurSubName;	
	for (int i = 0;i<SubNumbers.size();i++)
	{
		char buf[20];
		sprintf(buf, "%d,", SubNumbers[i]);
		request.SubNumbers += buf;		
	}
	m_client.Send(request.ToStr());
	LOG_FILE("TableId:%d   获取历史数据:%s", nTableId, request.ToStr().c_str());
	
}
void AppClient::GetDataSerial(int nTableId, int nSetNumber, int nSubNumber)
{
	if (nSetNumber > 20 || nSubNumber > 100)
	{
		return;
	}
	m_mutex.lock();
	m_TableDataSerial[nTableId] = "";
	m_mutex.unlock();
	DataSerialCheckRequest request;
	request.SeqNum = m_Seq++;
	request.TableId = nTableId;
	request.SetNumber = nSetNumber;
	request.SubNumber = nSubNumber;
	request.Date = GetDesktopCapture()->m_strPlayTime;
	m_client.Send(request.ToStr());
	LOG_FILE("TabldId:%d %d:%d 获取DataSerial", nTableId, nSetNumber, nSubNumber);
	int nStart = time(NULL);
	while (1)
	{
		Sleep(100);
		std::string strDataSerial = GetDataSerialStr(nTableId);
		if (strDataSerial.length() > 0)
		{
			LOG_FILE("TabldId:%d %02d:%02d 获取DataSerial成功 DataSerial:%s", nTableId, nSetNumber, nSubNumber, strDataSerial.c_str());
			return;
		}
		if (time(NULL) - nStart > 3)
		{
			LOG_FILE("TabldId:%d %02d:%02d 获取DataSerial超时，可能失败 DataSerial:%s", nTableId, nSetNumber, nSubNumber, strDataSerial.c_str());
			return;
		}
		
	}
}
std::string AppClient::GetDataSerialStr(int nTableId)
{
	std::string str;
	m_mutex.lock();
	str = m_TableDataSerial[nTableId];
	m_mutex.unlock();
	return str;
}
void AppClient::UpdateMehtod(std::string DataSerial, int MethodLength, int nTableId)
{
	if (DataSerial.length() == 0)
	{
		return;
	}
	MethodUpdateRequest request;
	request.DataSerial = DataSerial;
	request.SeqNum = m_Seq++;
	request.MethodLength = MethodLength;
	request.TableId = nTableId;
	request.AIPlayEnable = gAppConfig.bEnableAiPlay;
	m_client.Send(request.ToStr());

}