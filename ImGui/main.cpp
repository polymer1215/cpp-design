#define _CRT_SECURE_NO_WARNINGS
#include <imgui.h>
#include <imgui-SFML.h>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>

#include <iostream>

#include "UI.h"
#include "ManagerSingleton.h"

int main() {
    // 初始化窗口 - 使用更合理的默认窗口大小
    sf::RenderWindow window(sf::VideoMode({ 1280, 800 }), "Contact Manager");
    window.setFramerateLimit(60);

    ImGui::SFML::Init(window);

    // 设置字体
    auto& io = ImGui::GetIO();
    io.Fonts->Clear();
    io.Fonts->AddFontFromFileTTF("SourceHanSansCN-Regular.otf", 24.f, nullptr, io.Fonts->GetGlyphRangesChineseFull());
    ImGui::SFML::UpdateFontTexture();

    // 自动加载已保存的数据
    if (ManagerSingleton::get_instance().loadData()) {
        std::cout << "Auto-loaded " << ManagerSingleton::get_instance().personList.size() << " contacts." << std::endl;
    } else {
        std::cout << "No saved data found or error loading data." << std::endl;
    }

    UI ui;
    sf::Clock deltaClock;

    // 窗口主循环
    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
        }

        ImGui::SFML::Update(window, deltaClock.restart());

        ui.drawUI();

        window.clear();
        ImGui::SFML::Render(window);
        window.display();
    }

    ImGui::SFML::Shutdown();
    return 0;
}