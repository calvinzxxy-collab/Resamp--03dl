# Security / compatibility

This project targets one specific 32-bit game/client combination.

The v0.1.2 stable source intentionally avoids unverified low-level GTA SA,
SA-MP and RakNet memory patches. Only the D3D9 overlay hooks are installed by
default.

Unknown `samp.dll` PE profiles are rejected instead of being patched with
guessed offsets.

When reporting a crash, include GTA SA version, SA-MP version, exception
address, module base address, and the list of loaded ASI/CLEO plugins.
