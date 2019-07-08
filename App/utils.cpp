#include "stdafx.h"
#include "utils.h"
#include <string>
#include "libyuv/libyuv.h"
#include <string.h>
#include<fstream>
#include <stdio.h>
#include <winreg.h>
#include "aes256.h"
#include <list>
#include "svpng.h"
#include "zlib.h"
#include "cmd.h"
#include<windows.h>
#include<mmsystem.h>
#include "desktopCapture.h"
#include "desktopCaptureSecond.h"
#include "Logger.h"
#pragma comment(lib, "WINMM.LIB")
int nScale = 8;
#define nSrcWidth 60
#define nSrcHeight 15
#define nScaleWidth nSrcWidth*nScale
#define nScaleHeight nSrcHeight*nScale
int bmp_write1(unsigned char *image, int imageWidth, int imageHeight, char *filename)
{

	unsigned char header[54] = {
		0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,
		54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 32, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0
	};

	long file_size = (long)imageWidth * (long)imageHeight * 4 + 54;
	header[2] = (unsigned char)(file_size & 0x000000ff);
	header[3] = (file_size >> 8) & 0x000000ff;
	header[4] = (file_size >> 16) & 0x000000ff;
	header[5] = (file_size >> 24) & 0x000000ff;

	long width = imageWidth;
	header[18] = width & 0x000000ff;
	header[19] = (width >> 8) & 0x000000ff;
	header[20] = (width >> 16) & 0x000000ff;
	header[21] = (width >> 24) & 0x000000ff;

	long height = imageHeight;
	header[22] = height & 0x000000ff;
	header[23] = (height >> 8) & 0x000000ff;
	header[24] = (height >> 16) & 0x000000ff;
	header[25] = (height >> 24) & 0x000000ff;

	char fname_bmp[128];
	sprintf(fname_bmp, "%s.bmp", filename);

	FILE *fp;
	if (!(fp = fopen(fname_bmp, "wb")))
		return -1;

	fwrite(header, sizeof(unsigned char), 54, fp);
	fwrite(image, sizeof(unsigned char), (size_t)(long)imageWidth * imageHeight * 4, fp);

	fclose(fp);
	return 0;
}
int bmp_sacle(const char *filename)
{
	FILE* file = fopen(filename, "ab+");
	if (file )
	{
		unsigned char* buf = new unsigned char[1024 * 1024];
		unsigned char* buf1 = new unsigned char[1024 * 1024];
		int len = fread(buf, 1, 54, file);
		len = fread(buf, 1, nSrcWidth * (nSrcHeight+2) * 4, file);
		fclose(file);
		libyuv::ConvertToARGB(buf, (nSrcHeight + 2)*nSrcWidth * 4, buf1, 60  * 4, 0, 0, nSrcWidth, nSrcHeight + 2, 60, 15, libyuv::kRotate0, libyuv::FOURCC_ARGB);
		//bmp_write(buf1, 60, 15, (char*)filename);
		libyuv::ARGBScale(buf1, nSrcWidth*4, nSrcWidth, nSrcHeight, buf, nScaleWidth * 4, nScaleWidth, nScaleHeight, libyuv::kFilterBox);
		remove(filename);
		std::string name = filename;
		name = replace_all(name, ".bmp", "");
		bmp_write1(buf, nScaleWidth, nScaleHeight, (char*)name.c_str());
		delete[] buf;
		delete[] buf1;
	}	 
	return 0;
}
int bmp_sacle15(const char *filename)
{
	FILE* file = fopen(filename, "ab+");
	if (file)
	{
		unsigned char* buf = new unsigned char[1024 * 1024];
		unsigned char* buf1 = new unsigned char[1024 * 1024];
		int len = fread(buf, 1, 54, file);
		len = fread(buf, 1, nSrcWidth * nSrcHeight * 4, file);
		fclose(file);

		libyuv::ARGBScale(buf, nSrcWidth * 4, nSrcWidth, nSrcHeight, buf1, nScaleWidth * 4, nScaleWidth, nScaleHeight, libyuv::kFilterBox);
		remove(filename);
		std::string name = filename;
		name = replace_all(name, ".bmp", "");
		bmp_write1(buf1, nScaleWidth, nScaleHeight, (char*)name.c_str());
		delete[] buf;
		delete[] buf1;
	}
	return 0;
}
int bmp_write(unsigned char *image, int imageWidth, int imageHeight, char *filename)
{
	unsigned char* buf = new unsigned char[imageHeight*imageWidth * 4];
	unsigned char* buf1 = new unsigned char[imageHeight*imageWidth * 4];
	libyuv::ARGBRotate(image, imageWidth * 4, buf, imageWidth * 4, imageWidth, imageHeight, libyuv::kRotate180);
	libyuv::ARGBMirror(buf, imageWidth * 4,		buf1, imageWidth * 4,imageWidth, imageHeight);
	delete[] buf;
	image = buf1;
	unsigned char header[54] = {
		0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,
		54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 32, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0
	};

	long file_size = (long)imageWidth * (long)imageHeight * 4 + 54;
	header[2] = (unsigned char)(file_size & 0x000000ff);
	header[3] = (file_size >> 8) & 0x000000ff;
	header[4] = (file_size >> 16) & 0x000000ff;
	header[5] = (file_size >> 24) & 0x000000ff;

	long width = imageWidth;
	header[18] = width & 0x000000ff;
	header[19] = (width >> 8) & 0x000000ff;
	header[20] = (width >> 16) & 0x000000ff;
	header[21] = (width >> 24) & 0x000000ff;

	long height = imageHeight;
	header[22] = height & 0x000000ff;
	header[23] = (height >> 8) & 0x000000ff;
	header[24] = (height >> 16) & 0x000000ff;
	header[25] = (height >> 24) & 0x000000ff;

	char fname_bmp[128];
	sprintf(fname_bmp, "%s.bmp", filename);

	FILE *fp;
	if (!(fp = fopen(fname_bmp, "wb")))
		return -1;

	fwrite(header, sizeof(unsigned char), 54, fp);
	fwrite(image, sizeof(unsigned char), (size_t)(long)imageWidth * imageHeight * 4, fp);

	fclose(fp);
	delete[] image;
	return 0;
}
int bmp_write_mem(unsigned char *image, int imageWidth, int imageHeight, char *outbuf)
{
	unsigned char* buf = new unsigned char[imageHeight*imageWidth * 4];
	unsigned char* buf1 = new unsigned char[imageHeight*imageWidth * 4];
	libyuv::ARGBRotate(image, imageWidth * 4, buf, imageWidth * 4, imageWidth, imageHeight, libyuv::kRotate180);
	libyuv::ARGBMirror(buf, imageWidth * 4, buf1, imageWidth * 4, imageWidth, imageHeight);
	delete[] buf;
	image = buf1;
	unsigned char header[54] = {
		0x42, 0x4d, 0, 0, 0, 0, 0, 0, 0, 0,
		54, 0, 0, 0, 40, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 32, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0
	};

	long file_size = (long)imageWidth * (long)imageHeight * 4 + 54;
	header[2] = (unsigned char)(file_size & 0x000000ff);
	header[3] = (file_size >> 8) & 0x000000ff;
	header[4] = (file_size >> 16) & 0x000000ff;
	header[5] = (file_size >> 24) & 0x000000ff;

	long width = imageWidth;
	header[18] = width & 0x000000ff;
	header[19] = (width >> 8) & 0x000000ff;
	header[20] = (width >> 16) & 0x000000ff;
	header[21] = (width >> 24) & 0x000000ff;

	long height = imageHeight;
	header[22] = height & 0x000000ff;
	header[23] = (height >> 8) & 0x000000ff;
	header[24] = (height >> 16) & 0x000000ff;
	header[25] = (height >> 24) & 0x000000ff;
	int outlen = 0;

	memcpy(outbuf, header, 54);
	memcpy(outbuf + 54, image, imageWidth * imageHeight * 4);

	delete[] image;
	return imageWidth * imageHeight * 4 + 54;
}
const char * base64char = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char * base64_encode(const unsigned char * bindata, char * base64, int binlength)
{
	int i, j;
	unsigned char current;

	for (i = 0, j = 0; i < binlength; i += 3)
	{
		current = (bindata[i] >> 2);
		current &= (unsigned char)0x3F;
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)(bindata[i] << 4)) & ((unsigned char)0x30);
		if (i + 1 >= binlength)
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bindata[i + 1] >> 4)) & ((unsigned char)0x0F);
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)(bindata[i + 1] << 2)) & ((unsigned char)0x3C);
		if (i + 2 >= binlength)
		{
			base64[j++] = base64char[(int)current];
			base64[j++] = '=';
			break;
		}
		current |= ((unsigned char)(bindata[i + 2] >> 6)) & ((unsigned char)0x03);
		base64[j++] = base64char[(int)current];

		current = ((unsigned char)bindata[i + 2]) & ((unsigned char)0x3F);
		base64[j++] = base64char[(int)current];
	}
	base64[j] = '\0';
	return base64;
}

int base64_decode(const char * base64, unsigned char * bindata)
{
	int i, j;
	unsigned char k;
	unsigned char temp[4];
	for (i = 0, j = 0; base64[i] != '\0'; i += 4)
	{
		memset(temp, 0xFF, sizeof(temp));
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i])
				temp[0] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 1])
				temp[1] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 2])
				temp[2] = k;
		}
		for (k = 0; k < 64; k++)
		{
			if (base64char[k] == base64[i + 3])
				temp[3] = k;
		}

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[0] << 2)) & 0xFC)) |
			((unsigned char)((unsigned char)(temp[1] >> 4) & 0x03));
		if (base64[i + 2] == '=')
			break;

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[1] << 4)) & 0xF0)) |
			((unsigned char)((unsigned char)(temp[2] >> 2) & 0x0F));
		if (base64[i + 3] == '=')
			break;

		bindata[j++] = ((unsigned char)(((unsigned char)(temp[2] << 6)) & 0xF0)) |
			((unsigned char)(temp[3] & 0x3F));
	}
	return j;
}
std::string encoder_pic(unsigned char *image, int imageWidth, int imageHeight)
{
	unsigned char* imageout = new unsigned char[imageWidth*imageHeight * 5 + 100];
	int imageoutoutlen = bmp_write_mem(image, imageWidth, imageHeight, (char*)imageout);
	uLongf blen = compressBound(imageoutoutlen);	/* 压缩后的长度是不会超过blen的 */
	unsigned char* buf = new unsigned char[blen];
	unsigned char* base64buf = new unsigned char[blen*2];
	/* 压缩 */
	int err = compress((Byte*)buf, &blen, (Byte*)imageout, imageoutoutlen);
	base64_encode( buf, (char*)base64buf,blen);
	std::string str = (char*)base64buf;

	
	delete[] imageout;
	delete[] buf;
	delete[] base64buf;
	return str;
}
void decoder_pic(std::string str)
{
	unsigned char* base64buf = new unsigned char[1024 * 1024];
	unsigned char* buf = new unsigned char[1024 * 1024];
	long size = 0;
	size = base64_decode(str.c_str(), base64buf);
	uLongf uncomprLen = 1024 * 1024;
	int err = uncompress(buf, &uncomprLen, (const Bytef *)base64buf, size);
	buf[size] = 0;
	FILE* file = fopen("./1.bmp", "ab+");
	fwrite(buf, 1, uncomprLen, file);
	fclose(file);
	delete[] buf;
	delete[] base64buf;
}
std::vector<std::string> splitWithStl(const std::string &str, const std::string &pattern)
{
	std::vector<std::string> resVec;

	if ("" == str)
	{
		return resVec;
	}
	//方便截取最后一段数据
	std::string strs = str + pattern;

	size_t pos = strs.find(pattern);
	size_t size = strs.size();

	while (pos != std::string::npos)
	{
		std::string x = strs.substr(0, pos);
		if (x.length() > 0)
		{
			resVec.push_back(x);
		}		
		strs = strs.substr(pos + 1, size);
		pos = strs.find(pattern);
	}

	return resVec;
}
std::vector<std::wstring> splitWithStl(const std::wstring &str, const std::wstring &pattern)
{
	std::vector<std::wstring> resVec;

	if (L"" == str)
	{
		return resVec;
	}
	//方便截取最后一段数据
	std::wstring strs = str + pattern;

	size_t pos = strs.find(pattern);
	size_t size = strs.size();

	while (pos != std::wstring::npos)
	{
		std::wstring x = strs.substr(0, pos);
		if (x.length() > 0)
		{
			resVec.push_back(x);
		}
		strs = strs.substr(pos + 1, size);
		pos = strs.find(pattern);
	}

	return resVec;
}
RECT StrToRect(const char* str)
{
	RECT rect_;
	std::vector<std::string> strlist = splitWithStl(str, ",");
	if (strlist.size() == 4)
	{
		rect_.left = atoi(strlist[0].c_str());
		rect_.top = atoi(strlist[1].c_str());
		rect_.right = atoi(strlist[2].c_str());
		rect_.bottom = atoi(strlist[3].c_str());
	}
	return rect_;

 }
POINT StrToPOINT(const char* str)
{
	POINT point_;
	std::vector<std::string> strlist = splitWithStl(str, ",");
	if (strlist.size() == 2)
	{
		point_.x = atoi(strlist[0].c_str());
		point_.y = atoi(strlist[1].c_str());
	}
	return point_;
}

std::string&   replace_all(std::string&   str, const   std::string&   old_value, const   std::string&   new_value)
{
	while (true) {
		std::string::size_type   pos(0);
		if ((pos = str.find(old_value)) != std::string::npos)
			str.replace(pos, old_value.length(), new_value);
		else   break;
	}
	return   str;
}
std::string&   replace_frist(std::string&   str, const   std::string&   old_value, const   std::string&   new_value)
{
	while (true) {
		std::string::size_type   pos(0);
		if ((pos = str.find(old_value)) != std::string::npos)
			str.replace(pos, old_value.length(), new_value);
		 break;
	}
	return   str;
}
std::wstring&   replace_all(std::wstring&   str, const   std::wstring&   old_value, const   std::wstring&   new_value)
{
	while (true) {
		std::wstring::size_type   pos(0);
		if ((pos = str.find(old_value)) != std::wstring::npos)
			str.replace(pos, old_value.length(), new_value);
		else   break;
	}
	return   str;
}
// std::string  GetCpuID() {
// 	std::string str;
// 	// 定义一些变量，用于存储 CPU 的 ID信息	    
// 	BYTE szCpu[16] = { 0 };
// 	UINT uCpuID = 0U;
// 	// 以写追加的方式打开用来保存结果的文件   
// 
// 	// 对文件指针的有效性进行判断	    
// 	// 调用汇编代码，获取 CPU ID 信息	    
// 	__asm {
// 		mov eax, 0
// 		cpuid
// 		mov dword ptr szCpu[0], ebx
// 		mov dword ptr szCpu[4], edx
// 		mov dword ptr szCpu[8], ecx
// 		mov eax, 1
// 		cpuid
// 		mov uCpuID, edx
// 	}
// 	str = (char*)&szCpu[0];
// 	return str;
// 	// 把结果输出到文件中    
// }
std::string  ReadFile(const char* filename)
{
	std::fstream fin(filename); //打开文件
	std::string ReadLine;
	std::string ret;
	while (getline(fin, ReadLine)) //逐行读取，直到结束
	{
		if (ReadLine.find("//") == std::string::npos)
		{
			ret += ReadLine;
		}
	}
	fin.close();
	return ret;
}
// std::string GetCpuID1() {
// 	CString VernderID;	
// 	CString CPUID1, CPUID2;	
// 	unsigned long s1, s2;	
// 	unsigned char vendor_id[] = "------------"; 
// 	__asm {		xor eax, eax      //eax=0:取Vendor信息			cpuid    //取cpu id指令，可在Ring3级使用			
// 		mov dword ptr vendor_id,ebx			
// 		mov dword ptr vendor_id[+4],edx			
// 		mov dword ptr vendor_id[+8],ecx	
// 	}	
// 	VernderID.Format(_T("%s-"),vendor_id);	
// 	__asm	{		
// 		mov eax,01h   //eax=1:取CPU序列号			
// 		xor edx,edx			
// 		cpuid			
// 		mov s1,edx			
// 		mov s2,eax	
// 	}	
// 	CPUID1.Format(_T("%08X%08X"),s1,s2);	
// 	__asm	{		
// 		mov eax,03h			
// 		xor ecx,ecx			
// 		xor edx,edx			
// 		cpuid			
// 		mov s1,edx			
// 		mov s2,ecx	
// 	}	
// 	if (s1  > 0|| s2 > 0)
// 	{
// 	}
// 	CPUID2.Format(_T("%08X%08X"),s1,s2);	
// 	std::string strCPUID = CPUID1 + CPUID2;			
// 	return strCPUID;
// }

std::string GetAppStartupPath()
{
#if defined(_WIN32)||defined(_WIN64)
	std::string szRootA;
	char szPath[MAX_PATH] = { 0 };
	DWORD dwLen = GetModuleFileNameA(NULL, szPath, MAX_PATH);
	if (dwLen == 0)
		return "";


	char szLongPath[MAX_PATH] = { 0 };
	GetLongPathNameA(szPath, szLongPath, MAX_PATH);

	for (DWORD i = dwLen - 1; i >= 0; --i)
	{
		if (szLongPath[i] == '\\')
		{
			szLongPath[i] = '\0';
			break;
		}
	}

	szRootA = szLongPath;
	szRootA += "\\";

	return szRootA;
#else
	return "";
#endif
}
bool checkisdigit(char* str,int len)
{
	for (int j = 0; j < len; j++) {
		if (str[j] >= -1 && str[j] <= 255)
		{
			if (!isdigit(str[j])) { //判断字符是否是数字
				return false;
			}
		}
		else
		{
			return false;
		}
		
	}
	return true;
}
/*-4  A
-44 B
4- C
23 D
54 E
3- F
-46 G
-48 H
-6 I
-63 J
4-4 K
-40 L
-8  M
6- N
4-40 O
-64 P
4-44 Q
4-6 R
0-  S
4-8 T
4-46 U
4-48 V
4-63 W
4-64 X
64 Y
*/

std::string handleResult(std::string result)
{
	result = replace_all(result, "A","-4");
	result = replace_all(result, "B", "-44");
	result = replace_all(result, "C", "4-");
	result = replace_all(result, "D", "23");
	result = replace_all(result, "E", "54");
	result = replace_all(result, "F", "3-");
	result = replace_all(result, "G", "-46");
	result = replace_all(result, "H", "-48");
	result = replace_all(result, "I", "-6");
	result = replace_all(result, "J", "-63");
	result = replace_all(result, "K", "4-4");
	result = replace_all(result, "L", "-40");
	result = replace_all(result, "M", "-8");
	result = replace_all(result, "N", "6-");
	result = replace_all(result, "O", "4-40");
	result = replace_all(result, "P", "-64");
	result = replace_all(result, "Q", "4-44");
	result = replace_all(result, "R", "4-6");
	result = replace_all(result, "S", "0-");
	result = replace_all(result, "T", "4-8");
	result = replace_all(result, "U", "4-46");
	result = replace_all(result, "V", "4-48");
	result = replace_all(result, "W", "4-63");
	result = replace_all(result, "X", "4-64");
	result = replace_all(result, "Y", "64");
	result = replace_all(result, "Z", "6-40");
	result = replace_all(result, "a", "6-44");
	result = replace_all(result, "b", "6-46");
	result = replace_all(result, "c", "6-48");
	result = replace_all(result, "d", "0-44");
	result = replace_all(result, "e", "0-46");
	result = replace_all(result, "f", "0-48");
	result = replace_all(result, "g", "6-63");
	result = replace_all(result, "h", "6-64");
	result = replace_all(result, "i", "5-");
	result = replace_all(result, "j", "34");
	result = replace_all(result, "k", "6-6");
	result = replace_all(result, "l", "0-40");
	result = replace_all(result, "m", "0-4");
	result = replace_all(result, "n", "6-4");
	result = replace_all(result, "o", "6-6");
	result = replace_all(result, "p", "44");
	result = replace_all(result, "r", "48");
	result = replace_all(result, "s", "9-");
	result = replace_all(result, "t", "8-");
	result = replace_all(result, "u", "9-4");
	result = replace_all(result, "v", "5-48");
	result = replace_all(result, "x", "6-4");
	result = replace_all(result, "y", "5-4");
	result = replace_all(result, "z", "0-6");
	result = replace_all(result, "+", "-1");
	result = replace_all(result, "(", "5-40");
	result = replace_all(result, "--", "-");
	result = replace_all(result, " ", "");
	char buf[1024];
	memset(buf, 0, 1024);
	if (result.length() == 0)
	{
		return "";
	}
	if (result[result.length() - 1] == '-')
	{
		memcpy(buf, result.c_str(), result.length() - 1);
		result = buf;
	}
	if (result.length() == 2)
	{
		int n = atoi(result.c_str());
		int n1 = n / 10;
		int n2 = n % 10;
		sprintf(buf, "%d-%d", n1, n2);
		result = buf;
	}
	else if (result.length() == 5)
	{
		std::string str;
		if (result[3] == '-')
		{
			memcpy(buf, result.c_str(), 3);
			memcpy(buf + 3, result.c_str() + 4, 1);
			result = buf;
		}
	}
	else if (result.length() == 6)
	{
		std::string str;
		if (result[4] == '-')
		{
			memcpy(buf, result.c_str(), 4);
			memcpy(buf + 4, result.c_str() + 5, 1);
			result = buf;
		}
	}
	std::vector<std::string> resultlist;
	resultlist = splitWithStl(result, "-");
	if (resultlist.size() <= 1)
	{
		if (result.size() == 4)
		{
			memcpy(buf, result.c_str(), 2);
			buf[2] = '-';
			memcpy(buf + 3, result.c_str() + 2, 2);
			result = buf;
		}else if (result.size() == 3)
		{
			int n = atoi(result.c_str());
			if (n/10 >= 19)
			{
				memcpy(buf, result.c_str(), 1);
				buf[1] = '-';
				memcpy(buf + 2, result.c_str() + 1, 2);
				result = buf;
			}
			else
			{
				if (result == "154")
				{
					result = "15-4";
				}

// 				memcpy(buf, result.c_str(), 2);
// 				buf[1] = '-';
// 				memcpy(buf + 2, result.c_str() + 2, 1);
// 				result = buf;
				return "";
			}
		}
	}
	return result;
}
std::string GetProductID()
{
	std::string str;
	HKEY hKey;
	LONG ReturnValue;
	DWORD type;
	DWORD dwSize;
	char  strBuf[100];

	//访问注册表目录
	ReturnValue = RegOpenKeyEx(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
		0, KEY_READ | KEY_WOW64_64KEY, &hKey);

	if (ReturnValue != ERROR_SUCCESS)
		return "";
	memset(strBuf, 0, sizeof(strBuf));

	//获取产品ID
	dwSize = 100;
	LONG lReturn = RegQueryValueEx(hKey, "ProductId", NULL,
		&type, (BYTE *)strBuf, &dwSize);

	str = strBuf;
	return str;
}
std::string aes_encoder(std::string str)
{
	std::string ret;
	ret = aes128encoder((unsigned char*)str.c_str());
	return ret;

}
std::string aes_decoder(std::string str)
{
	std::string ret;
	ret = aes128decoder((unsigned char*)str.c_str(), str.length());
	return ret;
}
static unsigned short const wCRC16Table[256] = {
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040 };



int CRC16(const unsigned char* pDataIn, int iLenIn, unsigned short* pCRCOut)
{
	unsigned short wResult = 0;
	unsigned short wTableNo = 0;
	int i = 0;
	for (i = 0; i < iLenIn; i++)
	{
		wTableNo = ((wResult & 0xff) ^ (pDataIn[i] & 0xff));
		wResult = ((wResult >> 8) & 0xff) ^ wCRC16Table[wTableNo];
	}

	*pCRCOut = wResult;
	return wResult;
}
static unsigned short crc16_table[256] = {
	0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf,
	0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7,
	0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e,
	0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876,
	0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd,
	0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5,
	0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c,
	0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974,
	0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb,
	0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3,
	0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a,
	0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72,
	0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9,
	0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1,
	0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738,
	0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70,
	0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7,
	0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff,
	0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036,
	0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e,
	0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5,
	0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd,
	0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134,
	0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c,
	0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3,
	0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb,
	0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232,
	0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a,
	0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1,
	0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9,
	0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330,
	0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

uint32 crc_check(uint8* data, uint32 length)
{
	unsigned short crc_reg = 0xFFFF;
	while (length--)
	{
		crc_reg = (crc_reg >> 8) ^ crc16_table[(crc_reg ^ *data++) & 0xff];
	}
	return (uint32)(~crc_reg) & 0x0000FFFF;
}
#define CRC_16_POLYNOMIALS            0x8005

unsigned short CRC16_3(unsigned char* pchMsg, unsigned short wDataLen) // 1. MSB first; 2. Initial value is 0000;
{
	unsigned char i, chChar;
	unsigned short wCRC = 0;
	while (wDataLen--)
	{
		chChar = *pchMsg++;
		wCRC ^= (((unsigned short)chChar) << 8);
		for (i = 0; i < 8; i++)
		{
			if (wCRC & 0x8000)
				wCRC = (wCRC << 1) ^ CRC_16_POLYNOMIALS;
			else
				wCRC <<= 1;
		}
	}

	return wCRC;
}
std::string strcrc= "9COU+0h6NjJPL/+fmEfC05jbBbbZ5bT3c+I1DX8kO9QLG0zziVKGIgKLiyn7NgVMF+p6xtzD+DfI04eJiECoylb49QDV2zzxWRQ37wxD4xS7SxHT8iDITJR4F0k1LD63qa7w5J6xdf9XtYs3dGpa1NMGGWwwWA8omKmCtcoX8qhUnnGBjhTKfgOpGeFIv5RG";
 int ToCRC16(char* data,int Length, bool reverse )
{
	int len = Length;
	if (len > 0)
	{
		unsigned short crc = 0xFFFF;

		for (int i = 0; i < len; i++)
		{
			crc = (unsigned short)(crc ^ (data[i]));
			for (int j = 0; j < 8; j++)
			{
				crc = (crc & 1) != 0 ? (unsigned short)((crc >> 1) ^ 0xA001) : (unsigned short)(crc >> 1);
			}
		}
		if (reverse)
		{
			byte hi = (byte)((crc & 0xFF00) >> 8);  //高位置
			byte lo = (byte)(crc & 0x00FF);         //低位置
			return hi + lo * 0x100;
		}
		return crc;
	}
	return 0;
}

std::string encoderMsg(std::string json)
{
	
	std::string aesstr = aes_encoder(json);
	char bufret[64800];
	unsigned short crclen = 0;
	crclen = ToCRC16(( char*)aesstr.c_str(), aesstr.length(),true);
	sprintf(bufret,"##%06d%s%04X\r\n", aesstr.length(), aesstr.c_str(), crclen);
	return bufret;
}
std::string decoderMsg(char* msg,int len)
{
	return aes_decoder(msg);
}
#define SLEEPTIME 100
#if 0
void mouse_click(int x, int y)
{
	if (IsStop())
	{
		return;
	}
	int nWidth	= SCREEN_WIDTH ; //获取屏幕宽度
	int nHeight = SCREEN_HEIGHT;//获取屏幕高度	
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, x * 65535 / nWidth, y * 65535 / nHeight, 0, 0);
	Sleep(SLEEPTIME);
}

void mouse_move(int x, int y)
{
	if (IsStop())
	{
		return;
	}
	int nWidth = SCREEN_WIDTH; //获取屏幕宽度
	int nHeight = SCREEN_HEIGHT;//获取屏幕高度	
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE, x * 65535 / nWidth, y * 65535 / nHeight, 0, 0);
	Sleep(SLEEPTIME*2);
}
void mouse_click_second(int x, int y)
{
	if (IsStop())
	{
		return;
	}
	int nWidth = SCREEN_WIDTH; //获取屏幕宽度
	int nHeight = SCREEN_HEIGHT;//获取屏幕高度	
								//SetCursorPos(x+ SCREEN_WIDTH, y);
	mouse_event(MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP, (x + nWidth) * 65535 / nWidth, y * 65535 / nHeight, 0, 0);
	Sleep(SLEEPTIME);
}
#else

void mouse_click_second(int x, int y)
{
	if (IsStop())
	{
		return;
	}
	for (int i = 0;i < 8;i++)
	{
		bool bclick = (GetDesktopCapture1()->m_pClickCanclSecond + i)->bclick;
		int x1, y1;
		x1 = (GetDesktopCapture1()->m_pClickCanclSecond + i)->x;
		y1 = (GetDesktopCapture1()->m_pClickCanclSecond + i)->y;
		if (x1 != -1 && y1 != -1 && !bclick)
		{
			(GetDesktopCapture1()->m_pClickCanclSecond + i)->bclick = true;
			mouse_click(x1, y1);
		}
	}
	double fx = (x+ SCREEN_WIDTH) * (65535.0f / SCREEN_WIDTH);
	double fy = y * (65535.0f / SCREEN_HEIGHT);
	INPUT  Input[1];
	memset(Input, 0,  sizeof(INPUT));
	Input[0].type = INPUT_MOUSE;
	Input[0].mi.dwFlags =  MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
	Input[0].mi.dx = fx;
	Input[0].mi.dy = fy;
	::SendInput(1, Input, sizeof(INPUT));
	Sleep(SLEEPTIME);
}
void mouse_move(int x, int y)
{
	if (IsStop())
	{
		return;
	}
	double fx = x * (65535.0f / SCREEN_WIDTH);
	double fy = y * (65535.0f / SCREEN_HEIGHT);
	INPUT  Input[1];
	memset(Input, 0,   sizeof(INPUT));
	Input[0].type = INPUT_MOUSE;
	Input[0].mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	Input[0].mi.dx = fx;
	Input[0].mi.dy = fy;
	::SendInput(1, Input, sizeof(INPUT));
	Sleep(200);
}
void mouse_click(int x, int y)
{
	if (IsStop())
	{
		return;
	}
	for (int i = 0;i<8;i++)
	{
		bool bclick = (GetDesktopCapture1()->m_pClickCancl + i)->bclick;
		int x1, y1;
		x1 = (GetDesktopCapture1()->m_pClickCancl + i)->x;
		y1 = (GetDesktopCapture1()->m_pClickCancl + i)->y;
		if (x1 != -1 &&y1!=-1&& !bclick)
		{
			(GetDesktopCapture1()->m_pClickCancl + i)->bclick = true;
			mouse_click(x1, y1);
		}
	}
	double fx = x * (65535.0f / SCREEN_WIDTH);
	double fy = y * (65535.0f / SCREEN_HEIGHT);
	INPUT  Input[1];
	memset(Input, 0, 1 * sizeof(INPUT));
	Input[0].type = INPUT_MOUSE;
	Input[0].mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
	Input[0].mi.dx = fx;
	Input[0].mi.dy = fy;
	::SendInput(1, Input, sizeof(INPUT));
	Sleep(SLEEPTIME);
}
#endif
void findfile(char* lpPath, std::vector<std::string> &fileList)
{
	char szFind[MAX_PATH];
	WIN32_FIND_DATA FindFileData;

	strcpy(szFind, lpPath);
	strcat(szFind, "\\*.*");

	HANDLE hFind = ::FindFirstFile(szFind, &FindFileData);
	if (INVALID_HANDLE_VALUE == hFind)    return;

	while (true)
	{
		if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			if (FindFileData.cFileName[0] != '.')
			{
				char szFile[MAX_PATH];
				strcpy(szFile, lpPath);
				strcat(szFile, "\\");
				strcat(szFile, (char*)(FindFileData.cFileName));
				findfile(szFile, fileList);
			}
		}
		else
		{
			//std::cout << FindFileData.cFileName << std::endl;  
			fileList.push_back(FindFileData.cFileName);
		}
		if (!FindNextFile(hFind, &FindFileData))    break;
	}
	FindClose(hFind);
}
CString GetCurrentTime2Str()
{
	CString str; //获取系统时间 　　  
	CTime tm; tm = CTime::GetCurrentTime();
	str = tm.Format("%Y%m%d%H%M%S");
	return str;
}
CString GetCurrentTime2Str1()
{
	CString str; //获取系统时间 　　  
	CTime tm; tm = CTime::GetCurrentTime();
	str = tm.Format("%Y%m%d%H%M%S");
	return str;
}
CString GetCurrentDate2Str()
{
	CString str; //获取系统时间 　　  
	CTime tm; tm = CTime::GetCurrentTime();
	str = tm.Format("%Y-%m-%d %H:%M");
	return str;
}
std::string g_strtok(char * string, const char * control)
{
	char strbuf[2048];
	unsigned int nstrLen = strlen(string);
	std::string str;
	const char* pEnd = strstr(string, control);
	if (pEnd != NULL)
	{
		int nControlLen = pEnd - string;
		memcpy(strbuf, string, nControlLen);
		*(strbuf + (pEnd - string)) = '\0';
		str = strbuf;
	}
	return str;
}
void httpParamToMap(const std::string&   strHttp, std::map<std::string, std::string>& sParam)
{
	std::string str;
	str = strHttp;

	while (1)
	{
		std::string str1, str2, str3, str4;
		strseparate((char*)str.c_str(), str1, str2, "&");

		strseparate((char*)str1.c_str(), str3, str4, "=");
		sParam[str3] = str4;
		str = str2;
		if (str == "")
		{
			break;
		}
	}

}
void strseparate(char* src, std::string& sfrist, std::string& ssecond, const char* chSeparate)
{
	std::string str = src;
	char* sret = strtok((char*)str.c_str(), chSeparate);
	if (sret)
	{
		sfrist = sret;
		if (strcmp(sret, src) != 0)
		{
			ssecond = (src + strlen(sret) + strlen(chSeparate));
		}

	}
}
std::string betType2Str(int betType)
{
	if (betType == 0)
	{
		return "闲";
	}
	else if (betType == 1)
	{
		return "庄";
	}
	else if (betType == 2)
	{
		return "和";
	}
	else if (betType == 3)
	{
		return "庄对";
	}
	else if (betType == 4)
	{
		return "闲对";
	}
	return "";
}
int GetResult(std::vector<int> results)
{
	for (int i = 0;i<results.size();i++)
	{
		if (0>= results[i] && 2<= results[i])
		{
			return results[i];
		}
	}
	return -1;
}
std::string betTypelist2Str(std::vector<int> betTypelist)
{
	std::string ret;
	for (int i = 0;i < betTypelist.size();i++)
	{
		ret += betType2Str(betTypelist[i]);
		ret += "";
	}
	return ret;
}
std::string GBKToUTF8(const std::string& strGBK)
{
	std::string strOutUTF8 = "";
	WCHAR * str1;
	int n = MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, NULL, 0);
	str1 = new WCHAR[n];
	MultiByteToWideChar(CP_ACP, 0, strGBK.c_str(), -1, str1, n);
	n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
	char * str2 = new char[n];
	WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
	strOutUTF8 = str2;
	delete[]str1;
	str1 = NULL;
	delete[]str2;
	str2 = NULL;
	return strOutUTF8;
}
std::string UTF8ToGBK(const std::string& strUTF8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	unsigned short * wszGBK = new unsigned short[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, (LPCTSTR)strUTF8.c_str(), -1, (LPWSTR)wszGBK, len);

	len = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, NULL, 0, NULL, NULL);
	char *szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)wszGBK, -1, szGBK, len, NULL, NULL);
	//strUTF8 = szGBK;  
	std::string strTemp(szGBK);
	delete[]szGBK;
	delete[]wszGBK;
	return strTemp;
}
std::string GetCodeString(int httpCode)
{
	static std::map<int, std::string> mapHttpCode;
	if (mapHttpCode.size() == 0)
	{
		mapHttpCode[200] = "OK";
		mapHttpCode[201] = "Created";
		mapHttpCode[202] = "Accepted";
		mapHttpCode[203] = "Non-Authoritative Information";
		mapHttpCode[204] = "No Content";
		mapHttpCode[205] = "Reset Content";
		mapHttpCode[206] = "Partial Content";

		mapHttpCode[300] = "Multiple Choices";
		mapHttpCode[301] = "Moved Permanently";
		mapHttpCode[302] = "Found";
		mapHttpCode[303] = "See Other";
		mapHttpCode[304] = "Not Modified";
		mapHttpCode[305] = "Use Proxy";
		mapHttpCode[307] = "Temporary Redirect";

		mapHttpCode[400] = "Bad Request";
		mapHttpCode[401] = "Unauthorized";
		mapHttpCode[402] = "Payment Required";
		mapHttpCode[403] = "Forbidden";
		mapHttpCode[404] = "Not Found";
		mapHttpCode[405] = "ethod Not Allowed";
		mapHttpCode[406] = "Not Acceptable";
		mapHttpCode[405] = "ethod Not Allowed";
		mapHttpCode[406] = "Not Acceptable";
		mapHttpCode[407] = "Proxy Authentication Required";
		mapHttpCode[408] = "Request Time-out";

		mapHttpCode[409] = "Conflict";
		mapHttpCode[410] = "Gone";
		mapHttpCode[411] = "Length Required";
		mapHttpCode[412] = "Precondition Failed";
		mapHttpCode[413] = "Request Entity Too Large";
		mapHttpCode[414] = "Request-URI Too Large";

		mapHttpCode[415] = "Unsupported Media Type";
		mapHttpCode[416] = "Requested range not satisfiable";
		mapHttpCode[417] = "Expectation Failed";


		mapHttpCode[500] = "Internal Server Error";
		mapHttpCode[501] = "Not Implemented";
		mapHttpCode[502] = "Bad Gateway";
		mapHttpCode[503] = "Service Unavailable";
		mapHttpCode[504] = "Gateway Time-out";
		mapHttpCode[505] = "HTTP Version not supported";
	}
	std::string strRet;
	std::map<int, std::string>::iterator  it = mapHttpCode.find(httpCode);
	if (it != mapHttpCode.end())
	{
		strRet = it->second;
	}
	return strRet;

}


int  GetScreenCount()
{
	//return 1;
	std::list<DISPLAY_DEVICE> devices;
	std::list<DEVMODE> modes;
	int devId = 0;
	bool ret = false;
	bool isPrimary = false;
	int PrimaryNum = 0;
	do
	{
		DISPLAY_DEVICE displayDevice;
		ZeroMemory(&displayDevice, sizeof(DISPLAY_DEVICE));
		displayDevice.cb = sizeof(displayDevice);

		ret = EnumDisplayDevices(NULL, devId, &displayDevice, 0);
		if (ret == true)
		{
			if ((displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
			{
				devices.push_back(displayDevice);
				isPrimary = ((displayDevice.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE) == DISPLAY_DEVICE_PRIMARY_DEVICE);
				if (isPrimary)
					PrimaryNum = devId;
			}
		}
		devId++;
	} while (ret);
	return devices.size();	
}
std::string queryMachineGUID()
{
	HKEY hKEY;//定义有关的hKEY,在查询结束时要关闭

			  //打开与路径data_Set相关的hKEY

	LPCTSTR data_Set = _T("SOFTWARE\\Microsoft\\Cryptography");

	//访问注册表，hKEY则保存此函数所打开的键的句柄
	const DWORD dwSize = 500;

	TCHAR addr_Get[dwSize];
	memset(addr_Get, 0, dwSize);
	DWORD dwType = REG_SZ;
	LSTATUS ret;//RegOpenKeyEx(HKEY_CURRENT_USER, data_Set, 0, KEY_WOW64_64KEY | KEY_READ, &hKEY);
	if (ERROR_SUCCESS == ::RegOpenKeyEx(HKEY_LOCAL_MACHINE, data_Set, 0, KEY_WOW64_64KEY | KEY_READ, &hKEY))

	{
		if (ret = ::RegQueryValueEx(hKEY, _T("MachineGuid"), 0, &dwType, (LPBYTE)addr_Get, (LPDWORD)&dwSize) != ERROR_SUCCESS)

		{

			AfxMessageBox(_T("错误：无法查询有关的注册表信息"));

		}
		//程序结束，关闭打开的hKEY
		::RegCloseKey(hKEY);

	}
	std::string str;
	str = (char*)addr_Get;
	str = replace_all(str, "-", "");
	return str;
}
#include <gdiplus.h>
#include <afxwin.h>
using namespace Gdiplus;
#pragma  comment(lib, "gdiplus.lib")
void test_rgb(void) {
	test_rgb1();
	unsigned char rgb[256 * 256 * 3], *p = rgb;
	unsigned x, y;
	FILE *fp = fopen("rgb.png", "wb");
	for (y = 0; y < 256; y++)
		for (x = 0; x < 256; x++) {
			*p++ = (unsigned char)x;    /* R */
			*p++ = (unsigned char)y;    /* G */
			*p++ = 128;                 /* B */
		}
	svpng(fp, 256, 256, rgb, 0);
	fclose(fp);
}
int   GetEncoderClsid(const   WCHAR*   format, CLSID*   pClsid)
{
	UINT     num = 0;                     //   number   of   image   encoders 
	UINT     size = 0;                   //   size   of   the   image   encoder   array   in   bytes 

	ImageCodecInfo*   pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if (size == 0)
		return   -1;     //   Failure 

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
		return   -1;     //   Failure 

	GetImageEncoders(num, size, pImageCodecInfo);

	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return   j;     //   Success 
		}
	}

	free(pImageCodecInfo);
	return   -1;     //   Failure 
}

int png_write(unsigned char *image, int imageWidth, int imageHeight, char *filename)
{
	Bitmap img(L"c:\\img.png");
    CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	img.Save(L"New.png", &pngClsid, NULL);
// 	CImage img;
// 	img.Create(imageWidth, imageHeight, 24);
// 	char* pnewimage = (char*) img.GetBits();
// 	memcpy(pnewimage, img, imageWidth*imageHeight * 3);
// 	img.Save()
	return 0;
}
std::string  smtpIp(char * url)
{
	HOSTENT * iphost;
	char ipconfig[20];
	std::string str;
	if ((iphost = gethostbyname(url)) != NULL)
	{
		int i = 0;

		while (iphost->h_addr_list[i])
		{
			char ip[20];
			memcpy(&ip, inet_ntoa(*((struct in_addr *)iphost->h_addr_list[i])), 20);
			printf("%s", ip);
			str = ip;
			memset(ip, '\0', 20);
			i++;
		}
	}
	return str;
}
int textzlib()
{
	char text[] = "zlib compress and uncompress test\nturingo@163.com\n2012-11-05\n";
	uLong tlen = strlen(text) + 1;	/* 需要把字符串的结束符'\0'也一并处理 */
	char* buf = NULL;
	uLong blen;

	/* 计算缓冲区大小，并为其分配内存 */
	blen = compressBound(tlen);	/* 压缩后的长度是不会超过blen的 */
	if ((buf = (char*)malloc(sizeof(char) * blen)) == NULL)
	{
		printf("no enough memory!\n");
		return -1;
	}

	/* 压缩 */
	if (compress((Byte*)buf, &blen, (Byte*)text, tlen) != Z_OK)
	{
		printf("compress failed!\n");
		return -1;
	}

	memset(text, 0, tlen);
	/* 解压缩 */
	if (uncompress((Byte*)text, &tlen, (Byte*)buf, blen) != Z_OK)
	{
		printf("uncompress failed!\n");
		return -1;
	}

	/* 打印结果，并释放内存 */
	printf("%s", text);
	if (buf != NULL)
	{
		free(buf);
		buf = NULL;
	}
	return 1;

}
extern std::string check_results(std::vector<BoardInfoRequest> resulstlist, std::vector <std::string> methods)
{
	int maxmethod = 0;
	std::string methodresult;
	for (int i = 0;i< methods.size();i++)
	{
		int result = check_result(resulstlist, methods[i]);
		if (result > maxmethod)
		{
			maxmethod = result;
			methodresult = methods[i];
		}
	}
	return methodresult;
}
extern int check_result(std::vector<BoardInfoRequest> resulstlist, std::string menthords)
{
	int nPrevBetType = 0;
	nPrevBetType = GetResult(resulstlist[0].BetResult);
	int methodindex = 0;
	for (int i = 1;i<resulstlist.size();i++)
	{
		if (nPrevBetType == 2)
		{
			nPrevBetType = GetResult(resulstlist[i].BetResult);
			continue;
		}
		char curmethod = *(menthords.c_str() + methodindex);
		int methodvalue = curmethod == '1'?1:0;
		int nPlayBetType = 0;
		if (methodvalue )
		{
			nPlayBetType = !nPrevBetType;
		}
		else
		{
			nPlayBetType = nPrevBetType;
		}
		if (nPlayBetType == GetResult(resulstlist[i].BetResult))
		{
			methodindex++;
			nPrevBetType = nPlayBetType;
		}
		else
		{
			return i;
		}
	}
	return resulstlist.size() - 1;
}
//宽字符串转普通字符串
char *wtoc(wchar_t *wText)
{
	DWORD dwNum = WideCharToMultiByte(CP_ACP, NULL, wText, -1, NULL, 0, NULL, FALSE);
	char *psText = NULL;
	psText = new char[dwNum];
	if (!psText)
	{
		delete[]psText;
		psText = NULL;
	}
	WideCharToMultiByte(CP_ACP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	return psText;
}
//获取剪切板内容
std::string getPlateStr() {
	OpenClipboard(NULL);
	HGLOBAL hGlobal = NULL;
	hGlobal = ::GetClipboardData(CF_UNICODETEXT);
	wchar_t *pGlobal = (wchar_t *)::GlobalLock(hGlobal);
	CloseClipboard();
	EmptyClipboard();
	return wtoc(pGlobal);
}

std::string GetCopy(bool BMian)
{
	if (IsStop())
	{
		return "";
	}
	if (OpenClipboard(NULL))//打开剪贴板  
	{
		EmptyClipboard();
		CloseClipboard();
	}
	if (BMian)
	{
		if (GetDesktopCapture()->nFlashX != 0)
		{
			mouse_click(GetDesktopCapture()->nFlashX, GetDesktopCapture()->nFlashY);
		}		
		mouse_click(3, 200);
		
	}
	else
	{		
		if (GetScreenCount() == 1)
		{
			return "";
		}
		if (GetDesktopCaptureSecond()->nFlashX != 0)
		{
			mouse_click_second(GetDesktopCaptureSecond()->nFlashX, GetDesktopCaptureSecond()->nFlashY);
		}		
		mouse_click_second(3, 200);
		
		
	}
	Sleep(200);
	
	//Sleep(100);
// 	INPUT input[8];
// 	memset(input, 0, sizeof(input));
// 	//设置模拟键盘输入
// 	input[0].type = input[1].type = input[2].type = input[3].type = INPUT_KEYBOARD;
// 	input[0].ki.wVk = input[2].ki.wVk = VK_CONTROL;
// 	input[1].ki.wVk = input[3].ki.wVk = 'A';
// 	// 释放按键，这非常重要
// 	input[2].ki.dwFlags = input[3].ki.dwFlags = KEYEVENTF_KEYUP;
// 	SendInput(4, input, sizeof(INPUT));
// 
// 	memset(input, 0, sizeof(input));
// 	//设置模拟键盘输入
// 	input[0].type = input[1].type = input[2].type = input[3].type = INPUT_KEYBOARD;
// 	input[0].ki.wVk = input[2].ki.wVk = VK_CONTROL;
// 	input[1].ki.wVk = input[3].ki.wVk = 'C';
// 	// 释放按键，这非常重要
// 	input[2].ki.dwFlags = input[3].ki.dwFlags = KEYEVENTF_KEYUP;
// 	SendInput(4, input, sizeof(INPUT));

	keybd_event(VK_CONTROL, (BYTE)0, 0, 0);
	keybd_event('A', (BYTE)0, 0, 0); //此处可以用  'A', (BYTE)65, 用'a'不起作用.
	keybd_event('A', (BYTE)0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, (BYTE)0, KEYEVENTF_KEYUP, 0);
	Sleep(30);
	
	
	keybd_event(VK_CONTROL, (BYTE)0, 0, 0);
	keybd_event('C', (BYTE)0, 0, 0);
	keybd_event('C', (BYTE)0, KEYEVENTF_KEYUP, 0);
	keybd_event(VK_CONTROL, (BYTE)0, KEYEVENTF_KEYUP, 0);
	Sleep(30);
// 	keybd_event(VK_CONTROL, (BYTE)0, 0, 0);
// 	keybd_event('C', (BYTE)0, 0, 0);
// 	keybd_event('C', (BYTE)0, KEYEVENTF_KEYUP, 0);
// 	keybd_event(VK_CONTROL, (BYTE)0, KEYEVENTF_KEYUP, 0);
// 	Sleep(80);

	
	std::string str;
	int trycount = 0;
NextTry:
	if (OpenClipboard(NULL))//打开剪贴板  
	{
		if (IsClipboardFormatAvailable(CF_UNICODETEXT))//判断格式是否是我们所需要  
		{

			HANDLE hClip;

			//读取数据  
			hClip = GetClipboardData(CF_UNICODETEXT);

			wchar_t *pGlobal = (wchar_t *)GlobalLock(hClip);
			if (pGlobal)
			{
				USES_CONVERSION;
				str = W2A(pGlobal);
			}
			
			GlobalUnlock(hClip);
			EmptyClipboard();
			CloseClipboard();
			
		}
		else if (IsClipboardFormatAvailable(CF_TEXT))//判断格式是否是我们所需要  
		{
			HANDLE hClip;

			//读取数据  
			hClip = GetClipboardData(CF_TEXT);
			
			char *pGlobal = (char *)GlobalLock(hClip);
			if (pGlobal)
			{
				str = pGlobal;
			}
			GlobalUnlock(hClip);
			EmptyClipboard();
			CloseClipboard();
			
		}
		else
		{
			CloseClipboard();
		}
	}
	if (str.length() == 0&& trycount<10)
	{
		trycount++;
		Sleep(100);
		goto NextTry;
	}
	return str;
}
// std::string GetCopySecond()
// {
// 	if (IsStop())
// 	{
// 		return "";
// 	}
// 	if (GetScreenCount() == 1)
// 	{
// 		return "";
// 	}
// 	if (OpenClipboard(NULL))//打开剪贴板  
// 	{
// 		EmptyClipboard();
// 		CloseClipboard();
// 	}
// 	mouse_click_second(GetDesktopCaptureSecond()->nFlashX, GetDesktopCaptureSecond()->nFlashY);
// 	Sleep(100);
// 	mouse_click_second(10, 20);
// 	Sleep(100);
// 	keybd_event(VK_CONTROL, (BYTE)0, 0, 0);
// 	keybd_event('A', (BYTE)0, 0, 0); //此处可以用  'A', (BYTE)65, 用'a'不起作用.
// 	keybd_event('A', (BYTE)0, KEYEVENTF_KEYUP, 0);
// 	keybd_event(VK_CONTROL, (BYTE)0, KEYEVENTF_KEYUP, 0);
// 	Sleep(50);
// 
// 	keybd_event(VK_CONTROL, (BYTE)0, 0, 0);
// 	keybd_event('C', (BYTE)0, 0, 0);
// 	keybd_event('C', (BYTE)0, KEYEVENTF_KEYUP, 0);
// 	keybd_event(VK_CONTROL, (BYTE)0, KEYEVENTF_KEYUP, 0);
// 	Sleep(50);
// 	keybd_event(VK_CONTROL, (BYTE)0, 0, 0);
// 	keybd_event('C', (BYTE)0, 0, 0);
// 	keybd_event('C', (BYTE)0, KEYEVENTF_KEYUP, 0);
// 	keybd_event(VK_CONTROL, (BYTE)0, KEYEVENTF_KEYUP, 0);
// 	int trycount = 0;
// NextTry:	 
// 	std::string str;
// 	trycount++;
// 	if (OpenClipboard(NULL))//打开剪贴板  
// 	{
// 		if (IsClipboardFormatAvailable(CF_UNICODETEXT))//判断格式是否是我们所需要  
// 		{
// 
// 			HANDLE hClip;
// 
// 			//读取数据  
// 			hClip = GetClipboardData(CF_UNICODETEXT);
// 
// 			wchar_t *pGlobal = (wchar_t *)GlobalLock(hClip);
// 			if (pGlobal)
// 			{
// 				USES_CONVERSION;
// 				str = W2A(pGlobal);
// 			}
// 			
// 			GlobalUnlock(hClip);
// 			EmptyClipboard();
// 			CloseClipboard();
// 
// 		}
// 		else if (IsClipboardFormatAvailable(CF_TEXT))//判断格式是否是我们所需要  
// 		{
// 			HANDLE hClip;
// 
// 			//读取数据  
// 			hClip = GetClipboardData(CF_TEXT);
// 
// 			char *pGlobal = (char *)GlobalLock(hClip);
// 			if (pGlobal)
// 			{
// 				str = pGlobal;
// 			}			
// 			GlobalUnlock(hClip);
// 			EmptyClipboard();
// 			CloseClipboard();
// 
// 		}
// 		else
// 		{
// 			CloseClipboard();
// 		}
// 	}
// 	if (str.length() == 0)
// 	{
// 		goto NextTry;
// 	}
// 	return str;
// }
void PlayWav(std::string filename)
{
	PlaySound(filename.c_str(), 0, SND_FILENAME);
}
bool IsHasBJL(std::wstring str)
{
	if (str.find(L"百家乐") == std::wstring::npos)
	{
		return false;
	}
	return true;
}
extern bool IsHasBJL(std::wstring str, int TableId)
{
	CStringW cstr;
	cstr.Format(L"百家乐%02d", TableId);
	if (str.find(LPCWSTR(cstr)) == std::wstring::npos)
	{
		return false;
	}
	return true;
}
bool IsHasBJL(std::string str)
{
	if (str.find("百家乐") == std::wstring::npos)
	{
		return false;
	}
	return true;
}
bool IsHasGPBJL(std::string str)
{
	if (str.find("投注限红") == std::wstring::npos)
	{
		return false;
	}
	return true;
}
int GetTableIdFormBJL(std::string str)
{
	CString str1;
	for (int i = 1;i<=9;i++)
	{
		str1.Format("百家乐%02d", i);
		if (str1 == str.c_str())
		{
			return i;
		}
	}
	return -1;
}
extern bool IsHasBJL(std::string str, int TableId)
{
	CString cstr;
	cstr.Format("百家乐%02d", TableId);
	if (str.find(LPCSTR(cstr)) == std::string::npos)
	{
		return false;
	}
	return true;
}
extern bool IsHasTBJ(std::string str)
{
	CString cstr;
	cstr.Format("特别奖");
	if (str.find(LPCSTR(cstr)) == std::string::npos)
	{
		return false;
	}
	return true;
}
extern bool IsZX(std::string str)
{
	if (str == "庄" || str == "闲")
	{
		return true;
	}
	return false;
}
static bool gAppStop = false;
extern bool IsStop()
{
	return gAppStop == true;
}
extern void SetStop()
{
	gAppStop = true;
}
extern int GetSetNumber(std::string DataSerial)
{
	std::vector<std::string> strlist =  splitWithStl(DataSerial,"-");
	if (strlist.size() == 4)
	{
		return atoi(strlist[3].c_str());
	}
	return 0;
}
extern const char* GetAIStr(int nAIPlay)
{
	if (nAIPlay == 0)
	{
		return "0";
	}
	else if (nAIPlay == 1)
	{
		return "1";
	}
	else if (nAIPlay == 2)
	{
		return "2";
	}
	return "";
}
void ReadAI(std::vector<AI_S>& ais, std::vector<AI_Money> &aimoneys)
{
	std::fstream fin("./AI.txt"); //打开文件
	std::string ReadLine;
	std::string ret;
	while (getline(fin, ReadLine)) //逐行读取，直到结束
	{
		ReadLine = replace_all(ReadLine, " ", "");
		//没有=
		if (ReadLine.find("=") == std::string::npos)
		{
			if (ReadLine.find("2") != std::string::npos || ReadLine.find("3") != std::string::npos)
			{
				char c = *(ReadLine.c_str());
				std::string ReadLine1 = "0";
				std::string ReadLine2 = "1";
				if (c == '2' || c== '3')
				{
					ReadLine1 += ReadLine;
					ReadLine2 += ReadLine;
					for (int i = 1; i < ReadLine1.length(); i++)
					{
						char c = *(ReadLine1.c_str() + i);
						if (c == '2')
						{
							*((char*)(ReadLine1.c_str() + i)) = *(ReadLine1.c_str() + i - 1);

						}
						else if (c == '3')
						{
							if (*(ReadLine1.c_str() + i - 1) == '0')
							{
								*((char*)(ReadLine1.c_str() + i)) = '1';
							}
							else if (*(ReadLine1.c_str() + i - 1) == '1')
							{
								*((char*)(ReadLine1.c_str() + i)) = '0';
							}

						}
					}
					AI_S ai_s;
					ai_s.rule = ReadLine1.substr(0, ReadLine1.length() - 1);
					ai_s.value = atoi(ReadLine1.c_str() + ReadLine1.length() - 1);
					ais.push_back(ai_s);
					for (int i = 1; i < ReadLine2.length(); i++)
					{
						char c = *(ReadLine2.c_str() + i);
						if (c == '2')
						{
							*((char*)(ReadLine2.c_str() + i)) = *(ReadLine2.c_str() + i - 1);

						}
						else if (c == '3')
						{
							if (*(ReadLine2.c_str() + i - 1) == '0')
							{
								*((char*)(ReadLine2.c_str() + i)) = '1';
							}
							else if (*(ReadLine2.c_str() + i - 1) == '1')
							{
								*((char*)(ReadLine2.c_str() + i)) = '0';
							}

						}
					}
					ai_s.rule = ReadLine2.substr(0, ReadLine2.length() - 1);
					ai_s.value = atoi(ReadLine2.c_str() + ReadLine2.length() - 1);
					ais.push_back(ai_s);
				}
				else
				{
					for (int i = 1;i<ReadLine.length();i++)
					{
						char c = *(ReadLine.c_str()+i);
						if (c == '2' )
						{
							*((char*)(ReadLine.c_str() + i)) = *(ReadLine.c_str() + i - 1);

						}
						else if(c=='3')
						{
							if (*(ReadLine.c_str() + i - 1) == '0')
							{
								*((char*)(ReadLine.c_str() + i)) = '1';
							}
							else if (*(ReadLine.c_str() + i - 1) == '1')
							{
								*((char*)(ReadLine.c_str() + i)) = '0';
							}

						}
					}
					AI_S ai_s;
					ai_s.rule = ReadLine.substr(0, ReadLine.length() - 1);
					ai_s.value = atoi(ReadLine.c_str() + ReadLine.length() - 1);
					ais.push_back(ai_s);
				}
			}
			else if (ReadLine.length() > 0)
			{
				AI_S ai_s;
				ai_s.rule = ReadLine.substr(0, ReadLine.length() - 1);
				ai_s.value = atoi(ReadLine.c_str() + ReadLine.length() - 1);
				ais.push_back(ai_s);
			}
		}
		else//有=
		{
			std::vector<std::string> strlist = splitWithStl(ReadLine, "=");
			if (strlist.size() == 4)
			{
				AI_Money aimoney;
				aimoney.nIndex = atoi(strlist[0].c_str());
				aimoney.nMoney = atoi(strlist[1].c_str());
				aimoney.nWinindex = atoi(strlist[2].c_str());
				aimoney.nLossIndex = atoi(strlist[3].c_str());
				aimoneys.push_back(aimoney);
			}
		}
	}
	fin.close();

}

 void ReadAI2(std::map<int,AI_Ttoal>& AI_Ttoals )
 {
	 std::fstream fin("./AI.txt"); //打开文件
	std::string ReadLine;
	std::string ret;
	while (getline(fin, ReadLine)) //逐行读取，直到结束
	{
		ReadLine = replace_all(ReadLine, " ", "");
		//没有=
		if(ReadLine.find("table:") != std::string::npos)
		{
			std::vector<AI_Methond> aimethods; 
			std::vector<AI_Money> imoneys;
			std::string method;
			ReadLine = replace_all(ReadLine, "table:", "");
			int tableId = atoi(ReadLine.c_str());
			while (getline(fin, ReadLine)) //逐行读取，直到结束
			{
				std::vector<std::string> strlist = splitWithStl(ReadLine, "=");
				if (strlist.size() == 4)
				{
					AI_Money aimoney;
					aimoney.nIndex = atoi(strlist[0].c_str());
					aimoney.nMoney = atoi(strlist[1].c_str());
					aimoney.nWinindex = atoi(strlist[2].c_str());
					aimoney.nLossIndex = atoi(strlist[3].c_str());
					aimoneys.push_back(aimoney);
				}
				else{
					break;
				}
			}
			while (getline(fin, ReadLine)) //逐行读取，直到结束
			{
				std::vector<std::string> strlist = splitWithStl(ReadLine, "=");
				if (strlist.size() == 3)
				{
					AI_Methond aimethod;
					aimethod.nIndex = atoi(strlist[0].c_str());
					aimethod.nWinindex = atoi(strlist[1].c_str());
					aimethod.nLossIndex = atoi(strlist[2].c_str());
					aimethods.push_back(aimethod);
				}
				else{
					break;
				}
			}
			if (getline(fin, ReadLine)) //逐行读取，直到结束
			{
				ReadLine = replace_all(ReadLine, " ", "");
				method = ReadLine;
			}
			if (getline(fin, ReadLine)) //逐行读取，直到结束
			{
				ReadLine = replace_all(ReadLine, " ", "");
				method += ReadLine;
			}
			AI_Ttoals[tableId].aimethods = aimethods;
			AI_Ttoals[tableId].aimoneys = aimoneys;
			AI_Ttoals[tableId].method = method;
			
		}
		
	}
	fin.close();
 }