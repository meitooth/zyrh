#ifndef __DEBUG_H__
#define __DEBUG_H__



#if defined(_WIN32)||defined(_WIN64)


		#ifdef _DEBUG

			#define _CRTDBG_MAP_ALLOC
			#include <stdlib.h>
			#include <crtdbg.h>

		#endif




		 #ifdef _DEBUG
			#define DEBUG_NEW   new( _NORMAL_BLOCK, __FILE__, __LINE__)
			 // Replace _NORMAL_BLOCK with _CLIENT_BLOCK if you want the
			 //allocations to be of _CLIENT_BLOCK type
		 #else
			#define DEBUG_NEW
		 #endif // _DEBUG


		#ifdef _DEBUG
			#define _CRT_CHECK  \
					 int nFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);\
					 nFlag |= (_CRTDBG_LEAK_CHECK_DF|_CRTDBG_ALLOC_MEM_DF|_CRTDBG_CHECK_ALWAYS_DF);\
					 _CrtSetDbgFlag(nFlag)
		#else
			#define _CRT_CHECK
		#endif


		//_CrtSetBreakAlloc 内存分配序号处断点


#endif //defined(_WIN32)||defined(_WIN64)

#endif //__DEBUG_H__

