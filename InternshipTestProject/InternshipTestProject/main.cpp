#include "stdafx.h"
#include "project_watcher.h"

int main(int argc, char *argv[])
{
	ILogger * logger_ptr = new FileLogger();
	cout << "Enter file name and arguments(c:\\windows\\notepad.exe by default, enter skip to continue): " << endl;
	string proj_name;
	cin>>proj_name;
	if(proj_name=="skip")
		proj_name = "c:\\windows\\notepad.exe";
	/*cout << "Enter number of arguments" << endl;
	char inp[256];
	cin.getline(inp,256);
	system("PAUSE");
	  std::vector<char*> v;
	  char* chars_array = strtok(inp, "#:");
    while(chars_array)
    {
        v.push_back(chars_array);
        chars_array = strtok(NULL, " ");
    }
	char **arg_arr = new char*[v.size()];
	for(int i=0;i<v.size();i++)
	{
		arg_arr[i] = v[i];
	}*/
	//char *proj_name ="c:\\windows\\notepad.exe";
	argv++;
	project_watcher pr_watch(proj_name,argv,logger_ptr);
	while(true)
	{
		string command;
		cin>>command;
		if (command=="exit") 
		{
			PostThreadMessage(pr_watch.get_process_id(), WM_QUIT, 0, 0);
			WaitForSingleObject(pr_watch.get_process_handle(), 3000);
			break;
		}else if(command=="id")
		{
			cout<<pr_watch.get_process_id()<<endl;
		}else if(command=="status")
		{
			pr_watch.get_process_status(1);
		}else if(command=="stop")
		{
			pr_watch.stop_process();
		}else if(command=="start")
		{
			pr_watch.restart_process();
		}
	}
	return 0;
}