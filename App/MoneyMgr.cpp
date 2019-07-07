#include "stdafx.h"
#include "MoneyMgr.h"
#include "Logger.h"
CMoneyMgr::CMoneyMgr()
{

}
CMoneyMgr::~CMoneyMgr()
{

}
void CMoneyMgr::SetBetMoney(int nTableId, int nMoney, int Index)
{
	m_mutex.lock();
	m_MoneyBetList[nTableId] = nMoney;
	m_MoneyBetIndex[nTableId] = Index;
	m_mutex.unlock();
}
void CMoneyMgr::SetBetResult(int nTableId, int nProfit)
{
	m_mutex.lock();
	LOG_FILE("添加利润 TabldId:%d %d", nTableId, nProfit);
	m_MoneyBetList.erase(nTableId);
	m_MoneyResultist[nTableId] = nProfit;
	m_mutex.unlock();
}
void CMoneyMgr::GetResultMoneyList(std::vector<int>& moneylist)
{
	m_mutex.lock();
	
	std::vector<int>  tmpmoneylist;
	for (auto v: m_MoneyResultist)
	{
		tmpmoneylist.push_back(v.second);
		moneylist.push_back(v.second);
	}
	if (tmpmoneylist.size() == 2)
	{
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1]);
	}
	else if (tmpmoneylist.size() == 3)
	{
		//2
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2]);
		//3
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[2]);
	}
	else if (tmpmoneylist.size() == 4)
	{
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[3]);
		//3
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[2]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[3]);
		//4
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[3]);
	}
	else if (tmpmoneylist.size() == 5)
	{
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[3] + tmpmoneylist[4]);

		//3
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[2]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[3] + tmpmoneylist[4]);
		
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[3] + tmpmoneylist[4]);
		//4
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[3] + tmpmoneylist[4]);
	}
	else if (tmpmoneylist.size() == 6)
	{
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[3] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[4] + tmpmoneylist[5]);
		//3
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[2]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[3] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[4] + tmpmoneylist[5]);

		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[3] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[4] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[3] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[4] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[3] + tmpmoneylist[4] + tmpmoneylist[5]);
		//4
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[3] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[4] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[3] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[4] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[3] + tmpmoneylist[4] + tmpmoneylist[5]);

		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[3] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[4] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[3] + tmpmoneylist[4] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[3] + tmpmoneylist[4] + tmpmoneylist[5]);
	}
	else if (tmpmoneylist.size() == 7)
	{
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[3] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[3] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[4] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[4] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[5] + tmpmoneylist[6]);

		//3
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[2]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[1] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[2] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[3] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[3] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[4] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[4] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[0] + tmpmoneylist[5] + tmpmoneylist[6]);

		
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[3]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[2] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[3] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[3] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[4] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[4] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[1] + tmpmoneylist[5] + tmpmoneylist[6]);

		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[3] + tmpmoneylist[4]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[3] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[3] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[4] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[4] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[2] + tmpmoneylist[5] + tmpmoneylist[6]);

		moneylist.push_back(tmpmoneylist[3] + tmpmoneylist[4] + tmpmoneylist[5]);
		moneylist.push_back(tmpmoneylist[3] + tmpmoneylist[4] + tmpmoneylist[6]);
		moneylist.push_back(tmpmoneylist[3] + tmpmoneylist[5] + tmpmoneylist[6]);

		moneylist.push_back(tmpmoneylist[4] + tmpmoneylist[5] + tmpmoneylist[6]);
	}
	m_mutex.unlock();
	CString str;
	for (auto v: moneylist)
	{
		char buf[1024];
		sprintf(buf, "%d, ", v);
		str += buf;
	}
	LOG_FILE("当前有结果的局数 %s",(LPCSTR)str);
	
}
void CMoneyMgr::clear(int nTableId)
{
	m_mutex.lock();
	m_MoneyBetList.erase(nTableId);
	m_MoneyResultist.erase(nTableId);
	m_MoneyBetIndex.erase(nTableId);
	m_mutex.unlock();
}
std::vector<int> CMoneyMgr::GetBetMoneyList(int nTable,int nindex)
{
	std::vector<int> moneylist;
	m_mutex.lock();
	for (auto v : m_MoneyBetIndex)
	{
		if (v.second == nindex)
		{
			moneylist.push_back(m_MoneyBetList[nTable]);
		}
	}
	m_mutex.unlock();
	return moneylist;
}