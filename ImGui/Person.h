// Person类：表示通讯录成员的数据结构

#pragma once
#include <string>

struct Person {
public:
	Person();
	Person(std::string& id, std::string& name, std::string& gender, std::string& cA, std::string& email, std::string& pN);
	~Person() = default;
	std::string id;
	std::string name;
	std::string gender;
	std::string communicationAddress;
	std::string email;
	std::string phoneNumber;
};
