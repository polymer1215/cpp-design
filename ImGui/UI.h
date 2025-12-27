// UI类：负责渲染UI元素并处理交互逻辑

#pragma once
#include <SFML/Window.hpp>
#include <vector>
#include <memory>

#include "Person.h"

class UI {
private:
	std::unique_ptr<std::vector<Person>> filteredPersonList = nullptr;
	std::string currentId = "";
	bool isFiltered = false;

	bool addWindowDraw = false;
	bool editWindowDraw = false;
	bool deleteWindowDraw = false;
	bool searchWindowDraw = false;

	bool cantAdd = false;
	bool idNotExist = false;
	bool showSuccessMessage = false;
	bool showDeleteConfirm = false;
	bool emptyFieldsError = false;
	std::string successMessage = "";
	int messageTimer = 0;

	static const int MAX_STR_LEN = 256;
	char new_id_buffer[MAX_STR_LEN] = "";
	char new_name_buffer[MAX_STR_LEN] = "";
	char new_gender_buffer[MAX_STR_LEN] = "";
	char new_phone_buffer[MAX_STR_LEN] = "";
	char new_email_buffer[MAX_STR_LEN] = "";
	char new_address_buffer[MAX_STR_LEN] = "";
	char new_search_buffer[MAX_STR_LEN] = "";


private:
	void drawMenuBar();
	void drawMainTable();
	void getPopup();
	void drawPopup();
	void drawAddPopup();
	void drawEditPopup();
	void drawDeletePopup();
	void drawSearchPopup();
	void displayTable(std::vector<Person>& personList);
	void clearInputBuffers();
	bool validateInputFields();
	void showSuccessNotification(const std::string& message);
	void updateMessageTimer();

public:
	void drawUI();

};