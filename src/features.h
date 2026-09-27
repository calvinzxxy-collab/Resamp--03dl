#pragma once

#include <string>

namespace resamp {

class Features final {
public:
    void Load();
    void Save() const;
    void RenderMenu();

    bool HideChat() const noexcept { return hide_chat_; }
    bool DisableRadio() const noexcept { return disable_radio_; }
    bool StaminaBar() const noexcept { return stamina_bar_; }
    bool SquareRadar() const noexcept { return square_radar_; }
    bool Grass() const noexcept { return grass_; }

private:
    static std::string IniPath();
    static bool ReadBool(const char* key, bool fallback);
    static void WriteBool(const char* key, bool value);

    bool hide_chat_{};
    bool disable_radio_{};
    bool stamina_bar_{};
    bool square_radar_{};
    bool grass_{true};
};

} // namespace resamp
