#pragma once  
#include <vector>
#include <string>
#include <windows.h>
#include <map>
#include "svpng.h"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT  1080
 extern  int bmp_sacle(const char *filename);
 extern int bmp_sacle15(const char *filename);
 extern  int bmp_write(unsigned char *image, int imageWidth, int imageHeight, char *filename);
 extern std::vector<std::string> splitWithStl(const std::string &str, const std::string &pattern);
 extern std::vector<std::wstring> splitWithStl(const std::wstring &str, const std::wstring &pattern);
 extern RECT StrToRect(const char* str);
 extern POINT StrToPOINT(const char* str);
 extern std::string&   replace_all(std::string&   str, const   std::string&   old_value, const   std::string&   new_value);
 extern std::wstring&   replace_all(std::wstring&   str, const   std::wstring&   old_value, const   std::wstring&   new_value);
 extern std::string  GetCpuID();
 extern std::string  ReadFile(const char* filename);
 extern std::string GetCpuID1();
 extern std::string GetAppStartupPath();
 extern bool checkisdigit(char* str, int len);
 extern std::string handleResult(std::string result);
 extern std::string GetProductID();
 extern std::string aes_encoder(std::string str);
 extern std::string aes_decoder(std::string str);
 extern int CRC16(const unsigned char* pDataIn, int iLenIn, unsigned short* pCRCOut);
 extern std::string encoderMsg(std::string json);
 extern std::string decoderMsg(char* msg,int len);
 extern void mouse_click(int x, int y);
 extern void findfile(char* lpPath, std::vector<std::string> &fileList);
 extern CString GetCurrentTime2Str();
 extern CString GetCurrentTime2Str1();
 extern std::string g_strtok(char * string, const char * control);
 extern void strseparate(char* src, std::string& sfrist, std::string& ssecond, const char* chSeparate);
 extern void httpParamToMap(const std::string&   strHttp, std::map<std::string, std::string>& sParam);
 extern std::string betType2Str(int betType);
 extern std::string betTypelist2Str(std::vector<int> betTypelist);
 extern std::string GBKToUTF8(const std::string& strGBK);
 extern std::string UTF8ToGBK(const std::string& strUTF8);
 extern std::string GetCodeString(int httpCode);
 extern CString GetCurrentDate2Str();
 extern int GetScreenCount();
 extern void mouse_click_second(int x, int y);
 extern std::string queryMachineGUID();
 extern int png_write(unsigned char *image, int imageWidth, int imageHeight, char *filename);
 extern std::string  smtpIp(char * url);
 extern int textzlib();
 extern void decoder_pic(std::string str);
 struct BoardInfoRequest;
 extern std::string encoder_pic(unsigned char *image, int imageWidth, int imageHeight);
 extern std::string check_results(std::vector<BoardInfoRequest> resulstlist, std::vector <std::string> menthords);
 extern int check_result(std::vector<BoardInfoRequest> resulstlist, std::string menthords);
 extern std::string GetCopy(bool BMian = 1);
 //extern std::string GetCopySecond();
 extern void PlayWav(std::string filename);
 extern void mouse_move(int x, int y);
 extern bool IsHasBJL(std::wstring str);
 extern bool IsHasBJL(std::wstring str,int Table);
 extern bool IsHasBJL(std::string str);
 extern bool IsHasBJL(std::string str, int Table);
 extern bool IsHasTBJ(std::string str);
 extern int GetTableIdFormBJL(std::string str);
 extern bool IsZX(std::string str);
 extern bool IsStop();
 extern void SetStop();
 extern int GetSetNumber(std::string DataSerial);
 extern std::string&   replace_frist(std::string&   str, const   std::string&   old_value, const   std::string&   new_value);
 extern const char* GetAIStr(int nAIPlay);
 extern bool IsHasGPBJL(std::string str);
 struct AI_S
 {
	 std::string rule;
	 int value;
 };
 struct AI_Money
 {
	 int nWinindex;
	 int nLossIndex;
	 int nIndex;
	 int nMoney;
 };
  struct AI_Method
 {
	 int nWinindex;
	 int nLossIndex;
	 int nIndex;
 };
 
 
 struct AI_Ttoal{
	 std::vector<AI_Method> aimethods;
	 std::vector <AI_Money>   aimoneys;
	 std::string  method;
	 
 };
 void ReadAI(std::vector<AI_S>& ais, std::vector<AI_Money> &imoneys);
 void ReadAI2(std::map<int,AI_Ttoal>& AI_Ttoals);
