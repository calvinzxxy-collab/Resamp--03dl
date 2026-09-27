#pragma once

#include "features.h"
#include "renderer.h"
#include "samp_dl.h"

namespace resamp {

class Plugin final {
public:
    static Plugin& Get();
    void Run();

private:
    Plugin();

    samp::Client samp_{};
    Features features_{};
    Renderer renderer_;
};

} // namespace resamp
