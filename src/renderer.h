#pragma once

#include <windows.h>

namespace resamp {

class Features;

class Renderer final {
public:
    explicit Renderer(Features& features) : features_(features) {}

    bool Install();
    void RequestShutdown() noexcept { shutting_down_ = true; }

    bool MenuOpen() const noexcept { return menu_open_; }
    void ToggleMenu() noexcept { menu_open_ = !menu_open_; }
    void RenderFeatureMenu();

    static Renderer* Instance() noexcept { return instance_; }

private:
    Features& features_;
    bool menu_open_{};
    bool initialized_{};
    bool shutting_down_{};

    static Renderer* instance_;
};

} // namespace resamp
