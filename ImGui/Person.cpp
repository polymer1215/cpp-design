#include <string>
#include "Person.h"

Person::Person() {
	id = "NONE";
	name = "NONE";
	gender = "NONE";
	communicationAddress = "NONE";
	email = "NONE";
	phoneNumber = "NONE";
}

Person::Person(std::string& id, std::string& name, std::string& gender, std::string& cA, std::string& email, std::string& pN) : id(id),
	name(name), gender(gender), communicationAddress(cA), email(email), phoneNumber(pN) {
}