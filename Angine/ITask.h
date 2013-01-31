#pragma once


class ITask
{
private:
	bool isDead;
	long priority;

public:
	ITask(void);
	virtual ~ITask(void);

	bool dead();
	void kill();
	long getPriority();

	virtual bool start() = 0;
	virtual void onSuspend();
	virtual void update() = 0;
	virtual void onResume();
	virtual void stop() = 0;


};

