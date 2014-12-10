#include "stdafx.h"
#pragma once
class ILogger
{
public:
	ILogger(void);
	~ILogger(void);
	virtual void write(string type,time_t time)=0;
};

