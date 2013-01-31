#include "Kernel.h"


int Kernel::execute()
{
	std::list<ITask*>::iterator it;

    // Process active tasks
	for (it = this->active.begin(); it != this->active.end();) {
		ITask *t = (*it);
		++it;

		if (!t->dead())
			t->update();
	}

    // Stop and remove dead tasks
	for (it = this->active.begin(); it != this->active.end();) {
		ITask *t = (*it);
		++it;

		if (t->dead()) {
			t->stop();
			active.remove(t);
			t = 0;
		}
	}

	return 0;
}


bool Kernel::add(ITask *t)
{
	if (!t->start())
		return false;

	std::list<ITask*>::iterator it;

	for (it = this->active.begin(); it != this->active.end(); ++it) {
		ITask *comp = (*it);

		if (comp->getPriority() > t->getPriority())
			break;
	}

	this->active.insert(it, t);

	return true;
}


void Kernel::resume(ITask *t)
{
	if (std::find(this->paused.begin(), this->paused.end(), t) != this->paused.end()) {
		t->onResume();

		this->paused.remove(t);

		std::list<ITask*>::iterator it;

		for (it = this->active.begin(); it != this->active.end(); ++it) {
			ITask *comp = (*it);

			if (comp->getPriority() > t->getPriority())
				break;
		}

		this->active.insert(it, t);
	}
}


void Kernel::remove(ITask *t)
{
	if (std::find(this->active.begin(), this->active.end(), t) != this->active.end()) {
		t->kill();
	}
}


void Kernel::flush()
{
	for (std::list<ITask*>::iterator it = this->active.begin(); it != this->active.end(); ++it) {
		(*it)->kill();
	}
}