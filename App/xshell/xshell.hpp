#pragma once

#if defined(_WIN32)||defined(_WIN64)
#include "win_shell.hpp"
#else
#include "linux_shell.hpp"
#endif

template<class _App>
class Xshell
{
public:
	Xshell(const std::string& service_name,const std::string& service_describe):
	  sh_(service_name,service_describe)
	{

	}
	~Xshell(void){}

	int Run(int argc, char* argv[])
	{
		std::set_new_handler(out_of_memory);
		return sh_.Run(argc,argv);
	}

private:
	static void out_of_memory()
	{
		Sleep(500);
	}

private:

#if defined(_WIN32)||defined(_WIN64)
	WinShell<_App> sh_;
#else
	LinuxShell<_App> sh_;
#endif
};



