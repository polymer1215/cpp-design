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
		
		// Show success message in menu bar
		if (showSuccessMessage && messageTimer > 0) {
			ImGui::SameLine();
			ImGui::Spacing();
			ImGui::SameLine();
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), successMessage.c_str());
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
