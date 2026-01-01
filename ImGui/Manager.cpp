#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <sstream>
#include <ctime>
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

bool Manager::saveData() {
    try {
        json j = personList;
        std::ofstream ofs("contacts.json");
        if (!ofs.is_open()) {
            return false;
        }
        ofs << j.dump(2);
        ofs.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving data: " << e.what() << std::endl;
        return false;
    }
}

bool Manager::loadData() {
    std::ifstream ifs("contacts.json");
    if (!ifs.is_open()) {
        return false;
    }
    try {
        json j;
        ifs >> j;
        personList = j.get<std::vector<Person>>();
        ifs.close();
        return true;
    }
    catch (const std::exception& e) {
        // If JSON parsing fails, keep the current list
        std::cerr << "Error loading data: " << e.what() << std::endl;
        return false;
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

// Helper function to escape CSV fields
std::string Manager::escapeCSVField(const std::string& field) {
    // If field contains comma, quote, or newline, wrap in quotes and escape quotes
    bool needsQuoting = field.find(',') != std::string::npos || 
                       field.find('"') != std::string::npos || 
                       field.find('\n') != std::string::npos;
    
    if (!needsQuoting) {
        return field;
    }
    
    std::string escaped = "\"";
    for (char c : field) {
        if (c == '"') {
            escaped += "\"\""; // Escape quotes by doubling them
        } else {
            escaped += c;
        }
    }
    escaped += "\"";
    return escaped;
}

// Helper function to check gender (case-insensitive)
bool Manager::isGender(const std::string& gender, const std::string& checkFor) const {
    if (gender.empty()) return false;
    
    // Convert to lowercase for comparison
    std::string lowerGender = gender;
    std::transform(lowerGender.begin(), lowerGender.end(), lowerGender.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    if (checkFor == "male") {
        return lowerGender == "男" || lowerGender == "male" || lowerGender == "m";
    } else if (checkFor == "female") {
        return lowerGender == "女" || lowerGender == "female" || lowerGender == "f";
    }
    return false;
}

void Manager::sortContacts(SortField field, bool ascending) {
    std::sort(personList.begin(), personList.end(), [field, ascending](const Person& a, const Person& b) {
        bool result = false;
        switch (field) {
            case SortField::ID:
                result = a.id < b.id;
                break;
            case SortField::NAME:
                result = a.name < b.name;
                break;
            case SortField::PHONE:
                result = a.phoneNumber < b.phoneNumber;
                break;
        }
        return ascending ? result : !result;
    });
}

void Manager::clearAllContacts() {
    personList.clear();
}

void Manager::exportToCSV(const std::string& filename) {
    exportToCSV(filename, personList);
}

void Manager::exportToCSV(const std::string& filename, const std::vector<Person>& contacts) {
    std::ofstream ofs(filename);
    if (!ofs.is_open()) {
        throw std::runtime_error("无法创建CSV文件");
    }
    
    // Write UTF-8 BOM (Byte Order Mark) for better Excel compatibility
    ofs << "\xEF\xBB\xBF"; // UTF-8 BOM: EF BB BF
    ofs << "编号,姓名,性别,电话,邮箱,地址\n";
    
    // Write data rows with proper CSV escaping
    for (const Person& person : contacts) {
        ofs << escapeCSVField(person.id) << ","
            << escapeCSVField(person.name) << ","
            << escapeCSVField(person.gender) << ","
            << escapeCSVField(person.phoneNumber) << ","
            << escapeCSVField(person.email) << ","
            << escapeCSVField(person.communicationAddress) << "\n";
    }
    
    ofs.close();
}

int Manager::getMaleCount() const {
    return std::count_if(personList.begin(), personList.end(), [this](const Person& p) {
        return isGender(p.gender, "male");
    });
}

int Manager::getFemaleCount() const {
    return std::count_if(personList.begin(), personList.end(), [this](const Person& p) {
        return isGender(p.gender, "female");
    });
}

bool Manager::importFromCSV(const std::string& filename) {
    std::ifstream ifs(filename);
    if (!ifs.is_open()) {
        throw std::runtime_error("无法打开CSV文件");
    }
    
    std::string line;
    // Skip header line
    if (!std::getline(ifs, line)) {
        throw std::runtime_error("CSV文件为空");
    }
    
    int importedCount = 0;
    int skippedCount = 0;
    int lineNumber = 1;
    
    while (std::getline(ifs, line)) {
        lineNumber++;
        if (line.empty()) continue;
        
        // Parse CSV line (simple parsing - doesn't handle quoted fields with commas)
        std::vector<std::string> fields;
        std::stringstream ss(line);
        std::string field;
        
        while (std::getline(ss, field, ',')) {
            // Trim whitespace
            size_t start = field.find_first_not_of(" \t\r\n");
            size_t end = field.find_last_not_of(" \t\r\n");
            if (start != std::string::npos && end != std::string::npos) {
                field = field.substr(start, end - start + 1);
            } else {
                field = "";
            }
            fields.push_back(field);
        }
        
        // Ensure we have at least 6 fields
        if (fields.size() < 6) {
            std::cerr << "Line " << lineNumber << ": Not enough fields, skipping" << std::endl;
            skippedCount++;
            continue;
        }
        
        // Create person from CSV fields
        Person newPerson;
        newPerson.id = fields[0];
        newPerson.name = fields[1];
        newPerson.gender = fields[2];
        newPerson.phoneNumber = fields[3];
        newPerson.email = fields[4];
        newPerson.communicationAddress = fields[5];
        
        // Validate required fields
        if (newPerson.id.empty() || newPerson.name.empty() || newPerson.phoneNumber.empty()) {
            std::cerr << "Line " << lineNumber << ": Missing required fields, skipping" << std::endl;
            skippedCount++;
            continue;
        }
        
        // Check if ID already exists
        if (idExist(newPerson)) {
            std::cerr << "Line " << lineNumber << ": ID " << newPerson.id << " already exists, skipping" << std::endl;
            skippedCount++;
            continue;
        }
        
        // Add person
        personList.push_back(newPerson);
        importedCount++;
    }
    
    ifs.close();
    
    std::cout << "Import completed: " << importedCount << " imported, " << skippedCount << " skipped" << std::endl;
    return importedCount > 0;
}

bool Manager::createBackup() {
    try {
        json j = personList;
        // Get current time for backup filename
        auto now = std::time(nullptr);
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%Y%m%d_%H%M%S", std::localtime(&now));
        
        std::string backupFilename = std::string("contacts_backup_") + timeStr + ".json";
        std::ofstream ofs(backupFilename);
        if (!ofs.is_open()) {
            return false;
        }
        ofs << j.dump(2);
        ofs.close();
        std::cout << "Backup created: " << backupFilename << std::endl;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error creating backup: " << e.what() << std::endl;
        return false;
    }
}

bool Manager::restoreFromBackup() {
    // For simplicity, restore from the most recent backup file
    // In a real application, you would let the user select which backup to restore
    std::ifstream ifs("contacts_backup.json");
    if (!ifs.is_open()) {
        return false;
    }
    try {
        json j;
        ifs >> j;
        personList = j.get<std::vector<Person>>();
        ifs.close();
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error restoring backup: " << e.what() << std::endl;
        return false;
    }
}

std::vector<std::pair<Person, Person>> Manager::findDuplicates() const {
    std::vector<std::pair<Person, Person>> duplicates;
    
    // Check for duplicates by phone number or email
    for (size_t i = 0; i < personList.size(); i++) {
        for (size_t j = i + 1; j < personList.size(); j++) {
            const Person& p1 = personList[i];
            const Person& p2 = personList[j];
            
            // Check if phone or email match (and are not empty)
            bool phoneMatch = !p1.phoneNumber.empty() && p1.phoneNumber == p2.phoneNumber;
            bool emailMatch = !p1.email.empty() && !p2.email.empty() && p1.email == p2.email;
            bool nameMatch = p1.name == p2.name;
            
            // Consider it a potential duplicate if phone matches, or if name and email match
            if (phoneMatch || (nameMatch && emailMatch)) {
                duplicates.push_back(std::make_pair(p1, p2));
            }
        }
    }
    
    return duplicates;
}