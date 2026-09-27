#include "samp_dl.h"

#include <cstring>

namespace resamp::samp {

bool Client::IsReadable(const void* ptr, const std::size_t bytes) noexcept {
    if (!ptr || bytes == 0) return false;

    MEMORY_BASIC_INFORMATION mbi{};
    if (VirtualQuery(ptr, &mbi, sizeof(mbi)) != sizeof(mbi)) return false;
    if (mbi.State != MEM_COMMIT) return false;
    if ((mbi.Protect & PAGE_GUARD) != 0 || mbi.Protect == PAGE_NOACCESS) return false;

    const auto begin = reinterpret_cast<std::uintptr_t>(ptr);
    const auto end = begin + bytes;
    const auto region_end = reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
    return end <= region_end;
}

std::uint32_t Client::ReadPeEntryPoint(HMODULE module) noexcept {
    if (!module) return 0;

    const auto* base = reinterpret_cast<const std::uint8_t*>(module);
    if (!IsReadable(base, sizeof(IMAGE_DOS_HEADER))) return 0;

    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0) return 0;

    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + dos->e_lfanew);
    if (!IsReadable(nt, sizeof(*nt)) || nt->Signature != IMAGE_NT_SIGNATURE) return 0;
    return nt->OptionalHeader.AddressOfEntryPoint;
}

bool Client::Attach() {
    const HMODULE samp = GetModuleHandleA("samp.dll");
    if (!samp) return false;

    if (ReadPeEntryPoint(samp) != kDlR1.pe_entry_point) return false;

    base_ = reinterpret_cast<std::uintptr_t>(samp);
    return true;
}

void* Client::SampInfo() const noexcept {
    if (!base_) return nullptr;

    auto** slot = reinterpret_cast<void**>(SampInfoAddress());
    if (!IsReadable(slot, sizeof(void*))) return nullptr;
    return *slot;
}

void* Client::RakClient() const noexcept {
    auto* info = reinterpret_cast<std::uint8_t*>(SampInfo());
    if (!info) return nullptr;

    auto** slot = reinterpret_cast<void**>(info + kDlR1.rakclient_field);
    if (!IsReadable(slot, sizeof(void*))) return nullptr;
    return *slot;
}

} // namespace resamp::samp
