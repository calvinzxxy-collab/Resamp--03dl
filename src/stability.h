#pragma once

#include <cstdint>

namespace resamp::stability {

// Stable/test4 policy for SA-MP 0.3.DL-R1.
// These low-level hooks/patches stay disabled until independently validated.
inline constexpr bool kHookCGameProcess = false;          // GTA SA 1.0 US: 0x53BEE0
inline constexpr bool kApplyStartupPatchBundle = false;
inline constexpr bool kApplyVideoRefreshPatches = false;
inline constexpr bool kApplyVersionPatchList = false;
inline constexpr bool kInstallLowLevelRakNetHooks = false;

inline constexpr std::uintptr_t kCGameProcess = 0x53BEE0;
inline constexpr std::uintptr_t kObservedSampCrashOffset = 0x1A2B;

} // namespace resamp::stability
