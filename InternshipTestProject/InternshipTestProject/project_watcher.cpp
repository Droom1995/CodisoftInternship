#include "project_watcher.h"

int GetExitCodeProcessSmart(HANDLE hProcess, DWORD* pdwOutExitCode)
{
    int nRes = -1;

    DWORD dwExitCode = 0;
    if(::GetExitCodeProcess(hProcess, &dwExitCode))
    {
        if(dwExitCode != STILL_ACTIVE)
        {
            nRes = 1;
        }
        else
        {
            //Check if process is still alive
            DWORD dwR = ::WaitForSingleObject(hProcess, 0);
            if(dwR == WAIT_OBJECT_0)
            {
                nRes = 1;
            }
            else if(dwR == WAIT_TIMEOUT)
            {
                nRes = 0;
            }
            else
            {
                //Error
                nRes = -1;
            }
        }
    }

    if(pdwOutExitCode)
        *pdwOutExitCode = dwExitCode;

    return nRes;
}



project_watcher::project_watcher(string name,char ** args, ILogger *inp_logger)
{
	saved_args = args;
	process_path = name;
	event_logger = inp_logger;

	create_process(name,args);
	


	//thread t1(&project_watcher::process_watch,project_watcher());
	thread t1([&] (){
		this->process_watch(this);
	});
	t1.detach();
}


project_watcher::project_watcher()
{

}
project_watcher::~project_watcher(void)
{
}

unsigned project_watcher::get_process_id()
{
	return processInformation.dwProcessId;
}

HANDLE project_watcher::get_process_handle()
{
	return processInformation.hProcess;
}

DWORD project_watcher::get_process_status(int type)
{
	DWORD res;
	ZeroMemory(&res,sizeof(res));
	GetExitCodeProcessSmart(get_process_handle(),&res);
	if(res==STILL_ACTIVE)
	{
		if(type==1)
			cout<<"Process working"<<endl;
	}else if (res==0)
	{
		if(type==1)
			cout<<"Process stopped"<<endl;
	}else
		if(type==1)
			cout<<"Process crashed"<<endl;
	return res;
}

bool project_watcher::stop_process()
{
	return TerminateProcess(get_process_handle(),-1);
}
bool project_watcher::create_process(string name,char ** args)
{
	string AppName(name);
	started = true;

	//ZeroMemory(&processInformation,sizeof(processInformation));
	ZeroMemory(&startupInfo,sizeof(startupInfo));

    BOOL result;
	result = CreateProcess(
		AppName.c_str(),
		NULL,//L(args),
		NULL, 
		NULL, 
		FALSE, 
		NULL,
		NULL, 
		NULL, 
		&startupInfo, 
		&processInformation);
	closed = false;
	crashed = false;
	if(result)
		event_logger->write("Process created", time(0));
	return result;
}

bool project_watcher::restart_process()
{
	return create_process(process_path,saved_args);
}

void project_watcher::process_watch(project_watcher *prj)
{
	while(true)
	{
		DWORD status = get_process_status(0);
		if(status==STILL_ACTIVE)
		{
	//		mtx.lock();
			if(started)
			{
				prj->event_logger->write("Process started",time(0));
				prj->started = false;
				prj->closed=false;
				prj->crashed=false;
				on_proc_start();
			}
		//	mtx.unlock();
		}else if(status==0)
		{
		
			//mtx.lock();
			if(!closed)
			{
				prj->event_logger->write("Process stopped",time(0));
				prj->closed=true;
				prj->started = false;
				prj->crashed=false;
				on_proc_manually_stopped();
			}
			//mtx.unlock();
		}else
		{
		
			//mtx.lock();
			if(!crashed)
			{
				prj->event_logger->write("Process crashed",time(0));
				prj->crashed=true;
				prj->closed = false;
				prj->started = false;
				on_proc_crash();
			}
			//mtx.unlock();
		}
		chrono::milliseconds duration( 1000 );
		this_thread::sleep_for( duration );
	}
}

void project_watcher::on_proc_start()
{
	for(function<void()> *func:proc_start_callback)
		(*func)();
}
void project_watcher::on_proc_crash()
{
	for(function<void()> *func:proc_end_callback)
		(*func)();
}
void project_watcher::on_proc_manually_stopped()
{
	for(function<void()> *func:proc_crash_callback)
		(*func)();
}

void project_watcher::add_on_proc_start(function<void()> *func)
{
	proc_start_callback.push_back(func);
}

void project_watcher::add_on_proc_crash(function<void()> *func)
{
	proc_crash_callback.push_back(func);
}

void project_watcher::add_on_proc_manually_stopped(function<void()> *func)
{
	proc_end_callback.push_back(func);
}