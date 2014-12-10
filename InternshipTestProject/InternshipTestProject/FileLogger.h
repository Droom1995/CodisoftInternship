#pragma once
#include "ilogger.h"
#include "stdafx.h"
class FileLogger :
	public ILogger
{
	ofstream out;
public:
	FileLogger(void);
	~FileLogger(void);
	void write(string type,time_t time);
};

