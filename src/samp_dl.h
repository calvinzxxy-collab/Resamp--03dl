#pragma once

#include <cstddef>
#include <cstdint>
#include <windows.h>

namespace resamp::samp {

struct Profile {
    std::uintptr_t pe_entry_point;
    std::uintptr_t samp_info;
    std::uintptr_t rakclient_field;
};

inline constexpr Profile kDlR1{
    0x0FDB60,
    0x2ACA24,
    0x2C,
};

class Client final {
public:
    bool Attach();
    bool IsAttached() const noexcept { return base_ != 0; }
    std::uintptr_t Base() const noexcept { return base_; }
    std::uintptr_t SampInfoAddress() const noexcept { return base_ + kDlR1.samp_info; }
    void* SampInfo() const noexcept;
    void* RakClient() const noexcept;

private:
    static bool IsReadable(const void* ptr, std::size_t bytes) noexcept;
    static std::uint32_t ReadPeEntryPoint(HMODULE module) noexcept;

    std::uintptr_t base_{};
};

} // namespace resamp::samp
