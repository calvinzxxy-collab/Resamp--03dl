#include "features.h"

#include <windows.h>
#include <imgui.h>

namespace resamp {

std::string Features::IniPath() {
    char path[MAX_PATH]{};
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    std::string result(path);
    const auto slash = result.find_last_of("\\/");
    if (slash != std::string::npos) result.resize(slash + 1);
    result += "resamp++.ini";
    return result;
}

bool Features::ReadBool(const char* key, const bool fallback) {
    return GetPrivateProfileIntA("features", key, fallback ? 1 : 0, IniPath().c_str()) != 0;
}

void Features::WriteBool(const char* key, const bool value) {
    WritePrivateProfileStringA("features", key, value ? "1" : "0", IniPath().c_str());
}

void Features::Load() {
    hide_chat_ = ReadBool("hide_chat", false);
    disable_radio_ = ReadBool("disable_radio", false);
    stamina_bar_ = ReadBool("stamina_bar", false);
    square_radar_ = ReadBool("square_radar", false);
    grass_ = ReadBool("grass", true);
}

void Features::Save() const {
    WriteBool("hide_chat", hide_chat_);
    WriteBool("disable_radio", disable_radio_);
    WriteBool("stamina_bar", stamina_bar_);
    WriteBool("square_radar", square_radar_);
    WriteBool("grass", grass_);
}

void Features::RenderMenu() {
    ImGui::TextUnformatted("Stable 0.3DL clean-room build");
    ImGui::TextDisabled("Safe mode: game-loop/startup/video/RakNet hooks are disabled");
    ImGui::Separator();

    bool changed = false;
    changed |= ImGui::Checkbox("Hide chat", &hide_chat_);
    changed |= ImGui::Checkbox("Disable radio", &disable_radio_);
    changed |= ImGui::Checkbox("Stamina bar", &stamina_bar_);
    changed |= ImGui::Checkbox("Square radar", &square_radar_);
    changed |= ImGui::Checkbox("GTA grass", &grass_);

    if (changed) Save();

    ImGui::Spacing();
    ImGui::TextWrapped(
        "These switches are currently UI/config scaffolding in v0.1.2. "
        "The stable source does not install the risky CGame::Process, startup, "
        "video/refresh, version-patch-list, or low-level RakNet hooks until each "
        "one is independently verified for SA-MP 0.3.DL-R1."
    );
}

} // namespace resamp
