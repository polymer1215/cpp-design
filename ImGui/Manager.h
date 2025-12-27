// Manager类：负责维护通讯录成员列表的功能类

#pragma once
#include <vector>
#include "Person.h"

class Manager {
public:
	std::vector<Person> personList;
public:
	void loadData();
	void saveData();
	void deletePerson(Person& thePerson);
	bool addPerson(Person& newPerson);
	bool updatePerson(Person& updatedPerson);
	bool idExist(Person& thePerson);
	std::vector<Person> getFiltered(const std::string& key);

private:
	bool isSubStr(const std::string& sub, const std::string& main);
};