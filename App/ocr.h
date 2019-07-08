#include <string>
extern int doocr(const char* image, std::string& strout);
extern int doocrdigit(const char* image, std::string& strout);
extern int doocrcoutdown(const char* image, std::string& strout);
extern void doocrdigit_cmd(const char* image, std::string& strout);
extern void doocrcoutdown_cmd(const char* image, std::string& strout);
extern int execcmd(const char *cmd);
extern void doocr_cmd(const char* image, std::string& strout);
extern int doocrcancel(const char* image, std::string& strout);
extern int doocrtif(const char* image, std::string& strout, std::string outputname);
extern int doocrtifbox(const char* image, std::string& strout, std::string outputname);
