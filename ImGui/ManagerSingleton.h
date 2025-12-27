// ManagerSingleton类：Manager的一个单例，提供全局共享

#pragma once
#include "Manager.h"

class ManagerSingleton {
private:
	static Manager instance;
public:
	static Manager& get_instance();
};
