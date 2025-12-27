#include "ManagerSingleton.h"

Manager ManagerSingleton::instance;

Manager& ManagerSingleton::get_instance() {
	return instance;
}