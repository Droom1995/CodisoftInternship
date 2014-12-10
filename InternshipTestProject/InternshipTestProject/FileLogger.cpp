#include "FileLogger.h"


FileLogger::FileLogger(void)
{
	out.open("Log.txt");
}


FileLogger::~FileLogger(void)
{
}

void FileLogger::write(string event_type,time_t event_time)
{
	struct tm * timeinfo;
	timeinfo = localtime ( &event_time );
	out << event_type << ": " << asctime(timeinfo) << endl;
}
