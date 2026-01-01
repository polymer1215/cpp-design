// Manager类：负责维护通讯录成员列表的功能类

#pragma once
#include <vector>
#include "Person.h"

enum class SortField {
	ID,
	NAME,
	PHONE
};

class Manager {
public:
	std::vector<Person> personList;
public:
	bool loadData();
	bool saveData();
	void deletePerson(Person& thePerson);
	bool addPerson(Person& newPerson);
	bool updatePerson(Person& updatedPerson);
	bool idExist(Person& thePerson);
	std::vector<Person> getFiltered(const std::string& key);
	void sortContacts(SortField field, bool ascending = true);
	void clearAllContacts();
	void exportToCSV(const std::string& filename);
	void exportToCSV(const std::string& filename, const std::vector<Person>& contacts);
	int getMaleCount() const;
	int getFemaleCount() const;
	bool importFromCSV(const std::string& filename);
	bool createBackup();
	bool restoreFromBackup();
	std::vector<std::pair<Person, Person>> findDuplicates() const;

private:
	bool isSubStr(const std::string& sub, const std::string& main);
	std::string escapeCSVField(const std::string& field);
	bool isGender(const std::string& gender, const std::string& checkFor) const;
};