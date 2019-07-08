#ifndef STRINGZ_H
#define STRINGZ_H
#include <iostream>
#include <vector>
 
/* C++：去除左右空格 */
int trim_z(std::string& s);
 
/* C：去掉字符串首(左)空格函数  */
int ltrim_z(char* s);
 
/* C：去掉字符串尾(右)空格函数  */
int rtrim_z(char* s);
 
/* C：去掉字符串首尾空格函数  */
int trim_z(char* s);
 
/*  分割字符串
	@pch 需要分割的字符串
	@sp 分割符号，如：", : _ -"等
	@return vector<char*>
	例子：
	pch="姓名：张三"; sp=":"
	vector<char*> vec;
	vec[0] = "姓名";
	vec[1] = "张三";
*/
std::vector<char*> split_z(char* pch, char* sp);
 
/* 去掉右侧(一个或多个)换行符(\n)
*/
int rtrim_n_z(char* s);
 
/* 去掉左侧(一个或多个)换行符(\n)
*/
int ltrim_n_z(char* s);
 
/* 去掉左侧、右侧(一个或多个)换行符(\n)
*/
int trim_n_z(char* s);
 
#endif // !STRINGZ_H