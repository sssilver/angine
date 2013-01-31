#pragma once

#include <list>
#include <algorithm>

#include "ITask.h"


class Kernel
{
protected:
	std::list<ITask*> active;
	std::list<ITask*> paused;

public:
	int execute();

	bool add(ITask *t);
	void suspend(ITask *t);
	void resume(ITask *t);
	void remove(ITask *t);
	void flush();
};

