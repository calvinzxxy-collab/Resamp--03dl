# Reverse-engineering notes

These notes describe only what was used to create this clean-room scaffold. They are not a claim that this repository reproduces the original ReSAMP++ implementation.

## Supplied files

The analysis session used:

- `resamp++.asi`
  - PE32 / x86
  - SHA-256: `767fc0484402f0ea8bb8a9f83afba86813ff5f59355585ff3e426e4c9b3efc01`
- `samp.dll`
  - PE32 / x86
  - SHA-256: `bccdb297464bd382625635be25585df07a8fa6668bc0015650708e3eb4ffcd4b`
  - identified as SA-MP 0.3.DL-R1 during analysis

## Behavior/string observations

The supplied plugin binary contains UI/command strings indicating features around chat visibility, radio, stamina bar, square radar, GTA grass, visual effects, streaming-memory controls, water-FPS behavior, HP/armor indicators, nicknames/health bars, logging, INI configuration, and a texture resource named `resamp++.txd`.

This repository does not copy the binary's implementation. The feature list is used only as a roadmap for independent reimplementation.

## 0.3DL profile used by this repository

```text
PE entry point:  0x0FDB60
SAMP_INFO:       0x2ACA24
RakClient field: 0x2C
```

Independent public references:

- https://github.com/dotSILENT/chandling/blob/master/chandling_offsets.ini
- https://github.com/Rohatcengizhanbucak/OpenMPPlus/blob/main/docs/client-transport.md

## Porting rule

Do not add a feature just because an address appears plausible. For each feature:

1. Identify the exact structure/function on the target 0.3DL build.
2. Validate memory protection and surrounding instructions.
3. Keep the patch isolated in its own module.
4. Restore original bytes on unload/disable when applicable.
5. Fail closed on an unknown client build.

## v0.1.2 stability conclusion

The stable clean-room branch does not install the GTA `CGame::Process` hook at
`0x53BEE0`, startup/video patch bundles, version-dependent patch lists, or
low-level RakNet hooks.

One observed crash normalized to `samp.dll + 0x1A2B`.

This does not prove that a single original hook is universally broken. It means
the tested combination was not safe enough to ship as the stable baseline.
