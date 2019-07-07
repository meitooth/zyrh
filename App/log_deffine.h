#ifndef  _LOG_DEFINE_
#define  _LOG_DEFINE_
//============basic===================
#include <memory>
enum EnumLogLevel
{
	LogLevelTrace =  0,
	LogLevelDebug =  1,
	LogLevelInfo  =  2,  
	LogLevelWarring  =  3,
	LoglevelError =  4,
};
class Logger;
extern std::unique_ptr<Logger> g_logPtr;
extern std::unique_ptr<Logger> g_logPtr1;
extern std::unique_ptr<Logger> g_logPtr2;
 
#define  LOG_FUN ": [%s<%04d>] "
#define DEBUG_FORMAT(format,...) if(1) { printf(": [%s<%04d>] "format"\r\n",__FUNCTION__, __LINE__, ##__VA_ARGS__);}
#define LOG_ERROR(format,...)  if(g_logPtr) { g_logPtr->Write2Caching(LoglevelError,   ": [%s<%04d>] "format"\r\n",__FUNCTION__, __LINE__, ##__VA_ARGS__);}

#define LOG_INFO(format, ...)  if(g_logPtr) { g_logPtr->Write2Caching(LogLevelInfo,   ": [%s<%04d>] "format"\r\n",__FUNCTION__, __LINE__, ##__VA_ARGS__);}

#define LOG_DEBUG(format, ...)  if(g_logPtr) { g_logPtr->Write2Caching(LogLevelDebug,   ": [%s<%04d>] "format"\r\n",__FUNCTION__, __LINE__, ##__VA_ARGS__);}

#define  LOG_WARN(format, ...)  if(g_logPtr) { g_logPtr->Write2Caching(LogLevelWarring, ": [%s<%04d>] "format"\r\n",__FUNCTION__, __LINE__, ##__VA_ARGS__);}

#define LOG_TABLE(format, ...)  if(g_logPtr1) { g_logPtr1->Write2Caching(LogLevelInfo,   ": [%s<%04d>] "format"\r\n",__FUNCTION__, __LINE__, ##__VA_ARGS__);}

#define LOG_FILE(format, ...)  if(g_logPtr2) { g_logPtr2->Write2Caching(LogLevelInfo,   ": [%s<%04d>] "format"\r\n",__FUNCTION__, __LINE__, ##__VA_ARGS__);}



#define  WRITE_FILE(filename,buf,size) { FILE* file = fopen(filename,"ab+"); fwrite(buf,1,size,file);fclose(file);}
#endif
