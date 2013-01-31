#include "ITask.h"


ITask::ITask(void) : isDead(false), priority(5000)
{
}


ITask::~ITask(void)
{
}


bool ITask::dead()
{
	return this->isDead;
}


void ITask::kill()
{
	this->isDead = true;
}


long ITask::getPriority()
{
	return this->priority;
}


void ITask::onSuspend(void)
{
}


void ITask::onResume(void)
{
}