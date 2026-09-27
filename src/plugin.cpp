#include "plugin.h"

#include "stability.h"

#include <windows.h>

namespace resamp {

Plugin& Plugin::Get() {
    static Plugin instance;
    return instance;
}

Plugin::Plugin() : renderer_(features_) {}

void Plugin::Run() {
    // SA-MP may load after ASI plugins. Never access samp.dll too early.
    for (;;) {
        if (samp_.Attach()) break;
        Sleep(250);
    }

    // v0.1.2 stable/test4: keep risky low-level hooks disabled.
    static_assert(!stability::kHookCGameProcess);
    static_assert(!stability::kApplyStartupPatchBundle);
    static_assert(!stability::kApplyVideoRefreshPatches);
    static_assert(!stability::kApplyVersionPatchList);
    static_assert(!stability::kInstallLowLevelRakNetHooks);

    features_.Load();

    // Stable mode only installs the D3D9 overlay hooks.
    if (!renderer_.Install()) return;

    for (;;) Sleep(1000);
}

} // namespace resamp
