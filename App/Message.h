#pragma once
#include <vector>
#include <string>
struct  TabInfoMsg
{
	int nTableId = -1;
	std::vector<int>  Betlist;
	int nSetNumber = -1;
	int nSubNumber = -1;
	int nStatus = -1;
	int nBetTypeResult = -1;
	int CoinsBet = -1;
	int Profit = 0;
	int IsLost =-1; // 0 ”Æ 1 ‰ -1 ≤ª”Æ≤ª ‰
	int nTotaCoinsBetMoney = -1;
	int nTotalProfitMoney = -1;
	int nextBetType = 0;
	int nextAIPlay = PLAYNONE;
	int nAIplay = PLAYNONE;
	std::string nextCoinsBet;
	std::string message;
	std::string messageCoinsBet;
	std::string method;
	std::string nextmessage;

};