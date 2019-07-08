#pragma once
#include <string>


template<class _App>
class PPWorker
{
public:
	PPWorker(void){}
	~PPWorker(void){}

	void SetEventName(const char* event_name){
		event_name_=event_name;
	}

	bool Run()
	{
		HANDLE hEvent=OpenEventA(EVENT_ALL_ACCESS,FALSE,event_name_.c_str());
		if(!hEvent)
		{
			return false;
		}

		_App* p=new _App();
		if(!p->Init())
		{
			p->Uninit();
			delete p;
			return false;
		}

		WaitForSingleObject(hEvent,INFINITE);

		p->Uninit();
		delete p;

		CloseHandle(hEvent);

		return true;
	}
private:
	std::string event_name_;
};
