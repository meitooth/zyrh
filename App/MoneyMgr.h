#pragma once
#include <mutex>
#include <map>
#include <vector>
class CMoneyMgr
{
public:
	static CMoneyMgr* GetInstance()
	{
		static CMoneyMgr* p = NULL;
		if (p == NULL)
		{
			p = new CMoneyMgr;
		}
		return p;
	}
	CMoneyMgr();
	~CMoneyMgr();
	void SetBetMoney(int nTableId, int nMoney,int Index);
	void SetBetResult(int nTableId, int nProfit);
	void GetResultMoneyList(std::vector<int>& moneylist);
	void clear(int nTableId);
	std::vector<int> GetBetMoneyList(int nTableId,int nindex);
private:
	std::mutex m_mutex;
	std::map<int, int> m_MoneyBetIndex; //��ǰ��ע
	std::map<int, int> m_MoneyBetList; //��ǰ��ע
	std::map<int, int> m_MoneyResultist; //��ǰ���

};
