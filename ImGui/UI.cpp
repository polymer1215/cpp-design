#include <SFML/Window.hpp>
#include <imgui.h>
#include <iostream>

#include "UI.h"
#include "ManagerSingleton.h"


void UI::drawUI() {
	drawMenuBar();
	drawMainTable();
	getPopup();
	drawPopup();
	updateMessageTimer();
}

void UI::drawMainTable() {
	static bool main_window_open = true;
	static bool use_work_area = true;
	static ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoMove;
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(use_work_area ? viewport->WorkPos : viewport->Pos);
	ImGui::SetNextWindowSize(use_work_area ? viewport->WorkSize : viewport->Size, ImGuiCond_Always);

	if (ImGui::Begin("Contact Book", &main_window_open, flags)) {
		// Show contact count and filter status
		if (isFiltered) {
			ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), 
				u8"显示搜索结果: %zu 条 / 总共 %zu 条联系人", 
				filteredPersonList->size(), 
				ManagerSingleton::get_instance().personList.size());
			displayTable(*filteredPersonList);
		}
		else {
			ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), 
				u8"总共 %zu 条联系人", 
				ManagerSingleton::get_instance().personList.size());
			displayTable(ManagerSingleton::get_instance().personList);
		}

		ImGui::End();
	}

}
void UI::getPopup() {
	if (addWindowDraw) {
		ImGui::OpenPopup(u8"添加");
		addWindowDraw = false;
	}
	else if (editWindowDraw) {
		ImGui::OpenPopup(u8"修改");
		editWindowDraw = false;
	}
	else if (deleteWindowDraw) {
		ImGui::OpenPopup(u8"删除");
		deleteWindowDraw = false;
	}
	else if (searchWindowDraw) {
		ImGui::OpenPopup(u8"搜索");
		searchWindowDraw = false;
	}
	else if (sortWindowDraw) {
		ImGui::OpenPopup(u8"排序");
		sortWindowDraw = false;
	}
	else if (exportWindowDraw) {
		ImGui::OpenPopup(u8"导出");
		exportWindowDraw = false;
	}
	else if (statsWindowDraw) {
		ImGui::OpenPopup(u8"统计信息");
		statsWindowDraw = false;
	}
	else if (clearAllWindowDraw) {
		ImGui::OpenPopup(u8"清空所有联系人");
		clearAllWindowDraw = false;
	}

}
void UI::drawPopup() {
	ImVec2 center = ImGui::GetMainViewport()->GetCenter();
	ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

	if (ImGui::BeginPopupModal(u8"添加", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		drawAddPopup();
		ImGui::EndPopup();
	}

	else if (ImGui::BeginPopupModal(u8"修改", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		drawEditPopup();
		ImGui::EndPopup();
	}

	else if (ImGui::BeginPopupModal(u8"删除", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		drawDeletePopup();
		ImGui::EndPopup();
	}

	else if (ImGui::BeginPopupModal(u8"搜索", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		drawSearchPopup();
		ImGui::EndPopup();
	}

	else if (ImGui::BeginPopupModal(u8"排序", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		drawSortPopup();
		ImGui::EndPopup();
	}

	else if (ImGui::BeginPopupModal(u8"导出", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		drawExportPopup();
		ImGui::EndPopup();
	}

	else if (ImGui::BeginPopupModal(u8"统计信息", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		drawStatsPopup();
		ImGui::EndPopup();
	}

	else if (ImGui::BeginPopupModal(u8"清空所有联系人", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
		drawClearAllPopup();
		ImGui::EndPopup();
	}
}

void UI::drawAddPopup() {
	ImGui::InputText(u8"编号*", new_id_buffer, MAX_STR_LEN);
	ImGui::InputText(u8"姓名*", new_name_buffer, MAX_STR_LEN);
	ImGui::InputText(u8"性别", new_gender_buffer, MAX_STR_LEN);
	ImGui::InputText(u8"电话*", new_phone_buffer, MAX_STR_LEN);
	ImGui::InputText(u8"邮箱地址", new_email_buffer, MAX_STR_LEN);
	ImGui::InputText(u8"通讯地址", new_address_buffer, MAX_STR_LEN);
	ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), u8"标记*的字段为必填项");
	ImGui::Separator();

	if (ImGui::Button(u8"确定", ImVec2(120, 0)))
	{
		if (!validateInputFields()) {
			emptyFieldsError = true;
			cantAdd = false;
		}
		else if (!validatePhoneNumber(new_phone_buffer)) {
			showSuccessNotification(u8"电话号码格式不正确");
			emptyFieldsError = false;
			cantAdd = false;
		}
		else if (!validateEmail(new_email_buffer)) {
			showSuccessNotification(u8"邮箱格式不正确");
			emptyFieldsError = false;
			cantAdd = false;
		}
		else {
			Person newPerson;
			newPerson.id = new_id_buffer;
			newPerson.name = new_name_buffer;
			newPerson.gender = new_gender_buffer;
			newPerson.phoneNumber = new_phone_buffer;
			newPerson.email = new_email_buffer;
			newPerson.communicationAddress = new_address_buffer;
			if (ManagerSingleton::get_instance().addPerson(newPerson)) {
				clearInputBuffers();
				ImGui::CloseCurrentPopup();
				cantAdd = false;
				emptyFieldsError = false;
				showSuccessNotification(u8"联系人添加成功");
				performAutoSave();
			}
			else {
				cantAdd = true;
				emptyFieldsError = false;
			}
		}
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"取消", ImVec2(120, 0))) {
		clearInputBuffers();
		cantAdd = false;
		emptyFieldsError = false;
		ImGui::CloseCurrentPopup();
	}

	if (emptyFieldsError) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), u8"请填写所有必填字段");
	}
	if (cantAdd) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), u8"添加失败，编号已存在");
	}
}

void UI::drawEditPopup() {
	ImGui::InputText(u8"编号*", new_id_buffer, MAX_STR_LEN);
	ImGui::InputText(u8"姓名*", new_name_buffer, MAX_STR_LEN);
	ImGui::InputText(u8"性别", new_gender_buffer, MAX_STR_LEN);
	ImGui::InputText(u8"电话*", new_phone_buffer, MAX_STR_LEN);
	ImGui::InputText(u8"邮箱地址", new_email_buffer, MAX_STR_LEN);
	ImGui::InputText(u8"通讯地址", new_address_buffer, MAX_STR_LEN);
	ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), u8"输入编号后填写要修改的信息");
	ImGui::Separator();

	if (ImGui::Button(u8"确定", ImVec2(120, 0)))
	{
		if (!validateInputFields()) {
			emptyFieldsError = true;
			idNotExist = false;
		}
		else if (!validatePhoneNumber(new_phone_buffer)) {
			showSuccessNotification(u8"电话号码格式不正确");
			emptyFieldsError = false;
			idNotExist = false;
		}
		else if (!validateEmail(new_email_buffer)) {
			showSuccessNotification(u8"邮箱格式不正确");
			emptyFieldsError = false;
			idNotExist = false;
		}
		else {
			Person newPerson;
			newPerson.id = new_id_buffer;
			newPerson.name = new_name_buffer;
			newPerson.gender = new_gender_buffer;
			newPerson.phoneNumber = new_phone_buffer;
			newPerson.email = new_email_buffer;
			newPerson.communicationAddress = new_address_buffer;

			if (ManagerSingleton::get_instance().updatePerson(newPerson)) {
				clearInputBuffers();
				ImGui::CloseCurrentPopup();
				idNotExist = false;
				emptyFieldsError = false;
				showSuccessNotification(u8"联系人修改成功");
				performAutoSave();
			}
			else {
				idNotExist = true;
				emptyFieldsError = false;
			}
		}
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"取消", ImVec2(120, 0))) {
		clearInputBuffers();
		idNotExist = false;
		emptyFieldsError = false;
		ImGui::CloseCurrentPopup();
	}

	if (emptyFieldsError) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), u8"请填写所有必填字段");
	}
	if (idNotExist) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), u8"修改失败，编号不存在");
	}
}

void UI::drawDeletePopup() {
	ImGui::InputText(u8"编号", new_id_buffer, MAX_STR_LEN);
	ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.0f, 1.0f), u8"警告：删除操作不可恢复！");
	ImGui::Separator();
	
	if (ImGui::Button(u8"确定", ImVec2(120, 0)))
	{
		if (strlen(new_id_buffer) == 0) {
			idNotExist = true;
		}
		else {
			Person newPerson;
			newPerson.id = new_id_buffer;
			if (ManagerSingleton::get_instance().idExist(newPerson)) {
				ManagerSingleton::get_instance().deletePerson(newPerson);
				memset(new_id_buffer, 0, MAX_STR_LEN);
				ImGui::CloseCurrentPopup();
				idNotExist = false;
				showSuccessNotification(u8"联系人删除成功");
				performAutoSave();
			}
			else {
				idNotExist = true;
			}
		}
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"取消", ImVec2(120, 0)))
	{
		memset(new_id_buffer, 0, MAX_STR_LEN);
		idNotExist = false;
		ImGui::CloseCurrentPopup();
	}

	if (idNotExist) {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), u8"删除失败，编号不存在或为空");
	}
}

void UI::drawSearchPopup() {
	ImGui::InputText(u8"搜索", new_search_buffer, MAX_STR_LEN);
	ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), u8"可搜索：编号、姓名、性别、电话、邮箱、地址");
	ImGui::Separator();
	
	if (ImGui::Button(u8"确定", ImVec2(120, 0)))
	{
		std::string str = new_search_buffer;
		if (strlen(new_search_buffer) > 0) {
			filteredPersonList = std::make_unique<std::vector<Person>>(ManagerSingleton::get_instance().getFiltered(str));
			memset(new_search_buffer, 0, MAX_STR_LEN);
			ImGui::CloseCurrentPopup();
			isFiltered = true;
			
			// Show search result count
			std::string msg = u8"找到 " + std::to_string(filteredPersonList->size()) + u8" 条结果";
			showSuccessNotification(msg);
			std::cout << "Filtered: " << filteredPersonList->size() << " results." << std::endl;
		}
	}

	ImGui::SameLine();

	if (ImGui::Button(u8"取消", ImVec2(120, 0)))
	{
		memset(new_search_buffer, 0, MAX_STR_LEN);
		ImGui::CloseCurrentPopup();
	}

}

void UI::displayTable(std::vector<Person>& personList) {
	// Show empty state message if no contacts
	if (personList.empty()) {
		ImGui::Spacing();
		ImGui::Spacing();
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), u8"暂无联系人");
		ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), u8"点击 编辑 → 添加 来创建第一个联系人");
		return;
	}
	
	// Use flags for better table display
	ImGuiTableFlags flags = ImGuiTableFlags_Borders | 
	                        ImGuiTableFlags_RowBg | 
	                        ImGuiTableFlags_ScrollY |
	                        ImGuiTableFlags_SizingStretchProp;
	
	if (ImGui::BeginTable("contact_table", 6, flags)) {
		ImGui::TableSetupColumn(u8"编号");
		ImGui::TableSetupColumn(u8"姓名");
		ImGui::TableSetupColumn(u8"性别");
		ImGui::TableSetupColumn(u8"电话");
		ImGui::TableSetupColumn(u8"邮箱地址");
		ImGui::TableSetupColumn(u8"通讯地址");
		ImGui::TableHeadersRow();

		size_t nOfPerson = personList.size();
		for (size_t i = 0; i < nOfPerson; i++) {
			Person& person = personList[i];
			ImGui::TableNextRow();
			ImGui::TableSetColumnIndex(0);
			ImGui::Text(person.id.c_str());

			ImGui::TableSetColumnIndex(1);
			ImGui::Text(person.name.c_str());

			ImGui::TableSetColumnIndex(2);
			ImGui::Text(person.gender.c_str());

			ImGui::TableSetColumnIndex(3);
			ImGui::Text(person.phoneNumber.c_str());

			ImGui::TableSetColumnIndex(4);
			ImGui::Text(person.email.c_str());

			ImGui::TableSetColumnIndex(5);
			ImGui::Text(person.communicationAddress.c_str());
		}
		ImGui::EndTable();
	}
}

void UI::drawMenuBar() {
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu(u8"文件")) {
			if (ImGui::MenuItem(u8"加载")) {
				ManagerSingleton::get_instance().loadData();
				showSuccessNotification(u8"数据加载成功");
				std::cout << "Loaded successfully" << std::endl;
			}
			if (ImGui::MenuItem(u8"保存")) {
				ManagerSingleton::get_instance().saveData();
				showSuccessNotification(u8"数据保存成功");
				std::cout << "Saved successfully" << std::endl;
			}
			ImGui::Separator();
			if (ImGui::MenuItem(u8"导出到CSV")) {
				exportWindowDraw = true;
			}
			ImGui::Separator();
			if (ImGui::MenuItem(u8"自动保存", nullptr, &autoSaveEnabled)) {
				if (autoSaveEnabled) {
					showSuccessNotification(u8"已启用自动保存");
				} else {
					showSuccessNotification(u8"已禁用自动保存");
				}
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(u8"编辑")) {
			if (ImGui::MenuItem(u8"添加")) {
				addWindowDraw = true;
			}
			if (ImGui::MenuItem(u8"修改")) {
				editWindowDraw = true;
			}
			if (ImGui::MenuItem(u8"删除")) {
				deleteWindowDraw = true;
			}
			ImGui::Separator();
			if (ImGui::MenuItem(u8"清空所有联系人")) {
				clearAllWindowDraw = true;
			}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu(u8"搜索")) {
			if (ImGui::MenuItem(u8"使用搜索")) {
				searchWindowDraw = true;
			}
			if (ImGui::MenuItem(u8"重置搜索")) {
				isFiltered = false;
				filteredPersonList.reset();
				showSuccessNotification(u8"搜索已重置");
			}
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu(u8"工具")) {
			if (ImGui::MenuItem(u8"排序")) {
				sortWindowDraw = true;
			}
			if (ImGui::MenuItem(u8"统计信息")) {
				statsWindowDraw = true;
			}
			ImGui::EndMenu();
		}
		
		// Show success message in menu bar
		if (showSuccessMessage && messageTimer > 0) {
			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), successMessage.c_str());
		}
		
		// Show auto-save indicator
		if (autoSaveEnabled) {
			ImGui::SameLine();
			float menuBarWidth = ImGui::GetWindowWidth();
			float textWidth = ImGui::CalcTextSize(u8"[自动保存]").x;
			ImGui::SetCursorPosX(menuBarWidth - textWidth - 20);
			ImGui::TextColored(ImVec4(0.0f, 0.8f, 0.0f, 1.0f), u8"[自动保存]");
		}

		ImGui::EndMainMenuBar();
	}

}

// Helper function to clear all input buffers
void UI::clearInputBuffers() {
	memset(new_id_buffer, 0, MAX_STR_LEN);
	memset(new_name_buffer, 0, MAX_STR_LEN);
	memset(new_gender_buffer, 0, MAX_STR_LEN);
	memset(new_phone_buffer, 0, MAX_STR_LEN);
	memset(new_email_buffer, 0, MAX_STR_LEN);
	memset(new_address_buffer, 0, MAX_STR_LEN);
}

// Helper function to validate input fields (check for empty required fields)
bool UI::validateInputFields() {
	return strlen(new_id_buffer) > 0 && 
	       strlen(new_name_buffer) > 0 && 
	       strlen(new_phone_buffer) > 0;
}

// Helper function to show success notification
void UI::showSuccessNotification(const std::string& message) {
	successMessage = message;
	showSuccessMessage = true;
	messageTimer = 180; // Show for 3 seconds at 60 FPS
}

// Helper function to update message timer
void UI::updateMessageTimer() {
	if (messageTimer > 0) {
		messageTimer--;
		if (messageTimer == 0) {
			showSuccessMessage = false;
		}
	}
}

// Validation helper functions
bool UI::validateEmail(const std::string& email) {
	if (email.empty()) return true; // Optional field
	// Simple email validation: contains @ and at least one dot after @
	size_t atPos = email.find('@');
	if (atPos == std::string::npos) return false;
	size_t dotPos = email.find('.', atPos);
	return dotPos != std::string::npos && dotPos > atPos + 1 && dotPos < email.length() - 1;
}

bool UI::validatePhoneNumber(const std::string& phone) {
	if (phone.empty()) return false; // Required field
	// Check if phone contains only digits and optionally dashes/spaces
	for (char c : phone) {
		if (!isdigit(c) && c != '-' && c != ' ' && c != '+' && c != '(' && c != ')') {
			return false;
		}
	}
	// Check minimum length (at least 7 digits for a valid phone)
	int digitCount = 0;
	for (char c : phone) {
		if (isdigit(c)) digitCount++;
	}
	return digitCount >= 7;
}

// Auto-save helper
void UI::performAutoSave() {
	if (autoSaveEnabled) {
		ManagerSingleton::get_instance().saveData();
	}
}

// New popup implementations
void UI::drawSortPopup() {
	const char* sortOptions[] = { u8"编号", u8"姓名", u8"电话" };
	ImGui::Text(u8"选择排序字段：");
	ImGui::Combo(u8"排序字段", &sortFieldIndex, sortOptions, IM_ARRAYSIZE(sortOptions));
	
	ImGui::Checkbox(u8"升序排列", &sortAscending);
	
	ImGui::Separator();
	
	if (ImGui::Button(u8"确定", ImVec2(120, 0))) {
		SortField field;
		switch (sortFieldIndex) {
			case 0: field = SortField::ID; break;
			case 1: field = SortField::NAME; break;
			case 2: field = SortField::PHONE; break;
			default: field = SortField::ID;
		}
		
		ManagerSingleton::get_instance().sortContacts(field, sortAscending);
		
		std::string sortType = sortAscending ? u8"升序" : u8"降序";
		showSuccessNotification(u8"已按" + std::string(sortOptions[sortFieldIndex]) + sortType + u8"排序");
		ImGui::CloseCurrentPopup();
		performAutoSave();
	}
	
	ImGui::SameLine();
	
	if (ImGui::Button(u8"取消", ImVec2(120, 0))) {
		ImGui::CloseCurrentPopup();
	}
}

void UI::drawExportPopup() {
	ImGui::InputText(u8"文件名", export_filename_buffer, MAX_STR_LEN);
	ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), u8"导出为CSV格式");
	ImGui::Separator();
	
	if (ImGui::Button(u8"导出", ImVec2(120, 0))) {
		try {
			std::string filename = export_filename_buffer;
			// Ensure .csv extension
			if (filename.find(".csv") == std::string::npos) {
				filename += ".csv";
			}
			
			ManagerSingleton::get_instance().exportToCSV(filename);
			showSuccessNotification(u8"成功导出到 " + filename);
			ImGui::CloseCurrentPopup();
		}
		catch (const std::exception& e) {
			showSuccessNotification(u8"导出失败");
		}
	}
	
	ImGui::SameLine();
	
	if (ImGui::Button(u8"取消", ImVec2(120, 0))) {
		ImGui::CloseCurrentPopup();
	}
}

void UI::drawStatsPopup() {
	auto& manager = ManagerSingleton::get_instance();
	int total = manager.personList.size();
	int maleCount = manager.getMaleCount();
	int femaleCount = manager.getFemaleCount();
	int unspecified = total - maleCount - femaleCount;
	
	ImGui::Text(u8"联系人统计信息");
	ImGui::Separator();
	ImGui::Spacing();
	
	ImGui::Text(u8"总联系人数: %d", total);
	ImGui::Spacing();
	ImGui::Text(u8"男性: %d (%.1f%%)", maleCount, total > 0 ? (maleCount * 100.0f / total) : 0.0f);
	ImGui::Text(u8"女性: %d (%.1f%%)", femaleCount, total > 0 ? (femaleCount * 100.0f / total) : 0.0f);
	if (unspecified > 0) {
		ImGui::Text(u8"未指定性别: %d (%.1f%%)", unspecified, total > 0 ? (unspecified * 100.0f / total) : 0.0f);
	}
	
	ImGui::Separator();
	
	if (ImGui::Button(u8"关闭", ImVec2(240, 0))) {
		ImGui::CloseCurrentPopup();
	}
}

void UI::drawClearAllPopup() {
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), u8"警告：此操作将删除所有联系人！");
	ImGui::Text(u8"此操作不可恢复，确定要继续吗？");
	ImGui::Separator();
	
	if (ImGui::Button(u8"确定删除", ImVec2(120, 0))) {
		ManagerSingleton::get_instance().clearAllContacts();
		// Clear filter if active
		isFiltered = false;
		filteredPersonList.reset();
		showSuccessNotification(u8"已清空所有联系人");
		ImGui::CloseCurrentPopup();
		performAutoSave();
	}
	
	ImGui::SameLine();
	
	if (ImGui::Button(u8"取消", ImVec2(120, 0))) {
		ImGui::CloseCurrentPopup();
	}
}
