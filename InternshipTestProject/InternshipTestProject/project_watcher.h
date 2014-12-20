#pragma once
#include "stdafx.h"
#include "ILogger.h"
#include "FileLogger.h"

class project_watcher
{
	PROCESS_INFORMATION processInformation;
    STARTUPINFO startupInfo;
	string process_path;
	ILogger *event_logger;
	char ** saved_args;
	bool started,closed,crashed;
	mutex mtx;
	vector<function<void()> *> proc_start_callback,proc_crash_callback,proc_end_callback;
private:
	bool create_process(string name,char ** args);
public:
	project_watcher(string name,char ** args, ILogger *inp_logger);
	project_watcher();
	~project_watcher(void);
	unsigned get_process_id();
	HANDLE get_process_handle();
	DWORD get_process_status(int type);
	bool stop_process();
	bool restart_process();
	void process_watch(project_watcher *prj);

	void on_proc_start();
	void on_proc_crash();
	void on_proc_manually_stopped();
	void add_on_proc_start(function<void()> *func);
	void add_on_proc_crash(function<void()> *func);
	void add_on_proc_manually_stopped(function<void()> *func);
};

