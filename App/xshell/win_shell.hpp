#pragma once
#include "win_service.h"
#include "ppworker.hpp"
#include "arg_parser.h"
#include "dump.h"
#include "debug.h"

template<class _App>
class WinShell
{
public:
	WinShell(const std::string& service_name,const std::string& service_describe)
	{
		service_name_=service_name;
		service_describe_=service_describe;
	}
	~WinShell(void){}

	int Run(int argc, char* argv[])
	{
		_CRT_CHECK;
		DumpOpenDebugReport(_OnException);

		ArgParser ap;
		ap.Register("-i,--install");
		ap.Register("-u,--uninstall");
		ap.Register("-s,--service");
		ap.Register("-c,--console");
		ap.Register("-w,--work");
		ap.Register("-h,--help");

		ap.Register("-v,--event",true);

		//解析参数
		if(!ap.Parse(argc,argv))
		{
			printf("Command params error.\n");
			return -1;
		}

		if(ap.Get("-i")!="")
		{
			if(Install(ap))
			{
				printf("Install OK!\n");
			}
			else
			{
				printf("Installed failed. error=%d",GetLastError());
			}
		}
		else if(ap.Get("-u")!="")
		{
			if(Uninstall(ap))
			{
				printf("Uninstalled OK!\n");
			}
			else
			{
				printf("Uninstalled failed. error=%d",GetLastError());
			}
		}
		else if(ap.Get("-c")!="")
		{
			Console(ap);
		}
		else if(ap.Get("-s")!="")
		{
			WinService* s=WinService::GetInstance();
			s->SetServiceName(service_name_.c_str());
			s->Run();
			WinService::Release();
		}
		else if(ap.Get("-w")!="")
		{
			string szEventName=ap.Get("-v");

			PPWorker<_App> w;
			w.SetEventName(szEventName.c_str());
			w.Run();
		}
		else if(ap.Get("-h")!="")
		{
			printf("%s\n",ShowHelp().c_str());
		}

		return 0;
	}

private:

	//安装Windows服务
	bool Install(ArgParser& ap)
	{
		printf("Installing...\n");

		WinService* s=WinService::GetInstance();
		s->SetServiceName(service_name_.c_str());
		bool b=s->Install(service_describe_.c_str());
		WinService::Release();
		return b;
	}


	//卸载Windows服务
	bool Uninstall(ArgParser& ap)
	{
		printf("Uninstalling...\n");

		WinService* s=WinService::GetInstance();
		s->SetServiceName(service_name_.c_str());
		bool b=s->Uninstall();
		WinService::Release();
		return b;
	}

	//从控制台运行
	bool Console(ArgParser& ap)
	{
		 _App* p=new _App();
		if(!p->Init())
		{
			p->Uninit();
			delete p;
			return false;
		}

		while(getchar()!='x');

		p->Uninit();
		delete p;
		return true;
	}


	string ShowHelp()
	{
		string help;
		help+="XShell usage:\n";

		help+="-i/--install: Install a service.\n";
		help+="-u/--uninstall: Uninstall a service.\n";
		help+="-c/--console: Run server on console.\n";
		help+="-w/--work: Run work mode by a event.(Service mode only)\n";
		help+="    -v/--event: Event name.\n";
		help+="-s/--service: Run server on service.(Service mode only).\n";
		help+"-h/--help: Show help\n";
		return help;
	}


	static void __stdcall _OnException(struct _EXCEPTION_POINTERS* pException)
	{
		time_t t=time(NULL);
		tm _tm={0};
		localtime_s(&_tm,&t);
		char szTime[32]={0};
		_snprintf(szTime,31,"%04d%02d%02d%02d%02d%02d",_tm.tm_year+1900,_tm.tm_mon+1,_tm.tm_mday,_tm.tm_hour,_tm.tm_min,_tm.tm_sec);

		DumpDmpFile(szTime,pException);
		DumpStackFile(szTime,pException);

		TerminateProcess(GetCurrentProcess(),-1);
	}


private:
	std::string service_name_;
	std::string service_describe_;

};
