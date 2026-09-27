# resamp++ by Cyzenn

Open-source **clean-room reimplementation** of a ReSAMP++-style ASI plugin for **SA-MP 0.3.DL-R1 (x86)**.

> [!IMPORTANT]
> This is **not the original ReSAMP++ source code** and is not a decompilation presented as original source.
> It is a from-scratch implementation based on observable behavior, public SA-MP information, and compatibility testing.

## Version

**v0.1.2 — stable/test4 source**

This source follows the stability strategy from the latest black-screen investigation.

### Disabled in stable mode

The following low-level modifications are intentionally **not installed**:

- GTA `CGame::Process` hook at `0x53BEE0`
- startup GTA memory-patch bundle
- video / refresh-rate memory patches
- version-dependent memory patch list
- low-level SA-MP / RakNet hooks

These were kept out because they were the highest-risk areas during 0.3DL-R1 testing.

### Enabled foundation

- Win32/x86 ASI bootstrap
- waits for `samp.dll` before initialization
- SA-MP 0.3.DL-R1 PE entry-point validation
- 0.3DL pointer layer
- Direct3D 9 `EndScene` / `Reset` overlay hooks
- F2 menu
- Dear ImGui UI
- INI-backed settings scaffold
- GitHub Actions build

Known 0.3DL values:

```text
PE entry point  = 0x0FDB60
SAMP_INFO       = 0x2ACA24
RakClient field = 0x2C
```

## Stability note

One observed crash normalized to:

```text
samp.dll + 0x1A2B
```

Black-screen behavior was also seen while experimenting with startup and game-loop modifications.

For that reason, this repository uses a **fail-closed** approach: unverified low-level hooks are not installed.

## Build

Requirements:

- Windows
- Visual Studio 2022+
- Desktop development with C++
- CMake 3.21+
- Win32/x86 target

Commands:

```bat
cmake -S . -B build -A Win32
cmake --build build --config Release
```

Output:

```text
build/Release/resamp++.asi
```

Install it next to `gta_sa.exe`.

```text
GTA San Andreas/
├── gta_sa.exe
├── samp.dll
├── resamp++.asi
└── ...
```

Keep only **one** ReSAMP-style `.asi` in the folder while testing.

## Usage

1. Start SA-MP 0.3.DL-R1.
2. Join a server.
3. Wait until loading finishes.
4. Press **F2** to toggle the menu.

## Feature status

The current menu switches are a safe UI/configuration framework. They do not yet reproduce every function from the original closed ReSAMP++ binary.

Features should be ported and verified one by one instead of restoring large unverified memory patch groups.

## Project structure

```text
.
├── .github/workflows/build.yml
├── docs/reverse-engineering-notes.md
├── src/
│   ├── dllmain.cpp
│   ├── features.cpp
│   ├── features.h
│   ├── plugin.cpp
│   ├── plugin.h
│   ├── renderer.cpp
│   ├── renderer.h
│   ├── samp_dl.cpp
│   ├── samp_dl.h
│   ├── stability.h
│   └── version.h
├── CHANGELOG.md
├── CMakeLists.txt
├── LICENSE
├── README.md
└── SECURITY.md
```

## Compatibility

```text
GTA San Andreas: US 1.0
SA-MP:           0.3.DL-R1
Architecture:    x86 / 32-bit
```

## Credits

- ReSAMP++ / original author(s) — inspiration and original user-facing concept
- Dear ImGui
- MinHook
- GTA SA / SA-MP community reverse-engineering documentation

## Disclaimer

This project is independent and is not affiliated with Rockstar Games, Take-Two Interactive, SA-MP, or the original ReSAMP++ project.

## License

Original source written for this clean-room repository is released under the MIT License. See `LICENSE`.
