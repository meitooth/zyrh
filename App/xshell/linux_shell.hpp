#pragma once


#include <unistd.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <boost/thread.hpp>

bool bexit=false;

template<class _App>

class LinuxShell
{
public:

	LinuxShell(const std::string& service_name,const std::string& service_describe)
	{
	}

	~LinuxShell(void)
	{
	}

	int Run(int argc, char* argv[])
	{
		int pid=fork();


		if(pid<0)
			return pid;
		if(pid>0)
		{
			return 0;
		}

		setsid();
		signal(SIGCHLD, SIG_IGN);
		signal(SIGPIPE, SIG_IGN);
		signal(SIGINT, prog_exit);
		signal(SIGKILL, prog_exit);
		signal(SIGTERM, prog_exit);


		return Work();
		
	}

private:


	int Work()
	{
		_App* p=new _App();
		if(!p->Init())
		{
			p->Uninit();
			delete p;
			return -1;
		}

		while(!bexit)
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		}

		p->Uninit();
		delete p;

	}


	static void prog_exit(int iSignNo)
	{
		signal(SIGINT, SIG_IGN);
		signal(SIGKILL, SIG_IGN);
		signal(SIGTERM, SIG_IGN);
		bexit=true;
		printf("process_exit\n");

	}
};


