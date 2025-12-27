#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include "Manager.h"

using json = nlohmann::json;

void to_json(json& j, const Person& p) {
    j = json{
        {"id", p.id},
        {"name", p.name},
        {"gender", p.gender},
        {"communicationAddress", p.communicationAddress},
        {"email", p.email},
        {"phoneNumber", p.phoneNumber}
    };
}

void from_json(const json& j, Person& p) {
    p.id = j.at("id").get<std::string>();
    p.name = j.at("name").get<std::string>();
    p.gender = j.at("gender").get<std::string>();
    p.communicationAddress = j.at("communicationAddress").get<std::string>();
    p.email = j.at("email").get<std::string>();
    p.phoneNumber = j.at("phoneNumber").get<std::string>();
}

void Manager::saveData() {
    json j = personList;
    std::ofstream ofs("contacts.json");
    if (ofs.is_open()) {
        ofs << j.dump(2);
        ofs.close();
    }
}

void Manager::loadData() {
    std::ifstream ifs("contacts.json");
    if (!ifs.is_open()) {
        return;
    }
    try {
        json j;
        ifs >> j;
        personList = j.get<std::vector<Person>>();
        ifs.close();
    }
    catch (const std::exception& e) {
        // If JSON parsing fails, keep the current list
        std::cerr << "Error loading data: " << e.what() << std::endl;
    }
}

bool Manager::addPerson(Person& newPerson) {
    // Check if ID already exists
    if (idExist(newPerson)) {
        return false;
    }
    personList.push_back(newPerson);
    return true;
}

bool Manager::idExist(Person& thePerson) {
    for (const Person& person : personList) {
        if (person.id == thePerson.id) {
            return true;
        }
    }
    return false;
}

bool Manager::updatePerson(Person& updatedPerson) {
    for (Person& person : personList) {
        if (person.id == updatedPerson.id) {
            person.name = updatedPerson.name;
            person.gender = updatedPerson.gender;
            person.phoneNumber = updatedPerson.phoneNumber;
            person.email = updatedPerson.email;
            person.communicationAddress = updatedPerson.communicationAddress;
            return true;
        }
    }
    return false;
}

void Manager::deletePerson(Person& thePerson) {
    auto it = std::remove_if(personList.begin(), personList.end(), [&thePerson](const Person& person) {
        return person.id == thePerson.id;
        });

    personList.erase(it, personList.end());
}

std::vector<Person> Manager::getFiltered(const std::string& key) {
    std::vector<Person> filtered;
    filtered.reserve(personList.size()); // Reserve space for efficiency
    
    for (Person& person : personList) {
        if (isSubStr(key, person.id) ||
            isSubStr(key, person.name) ||
            isSubStr(key, person.email) ||
            isSubStr(key, person.gender) ||
            isSubStr(key, person.communicationAddress) ||
            isSubStr(key, person.phoneNumber)) {
            filtered.push_back(person);
        }
    }
    return filtered;
}

bool Manager::isSubStr(const std::string& sub, const std::string& main) {
    return main.find(sub) != std::string::npos;
}