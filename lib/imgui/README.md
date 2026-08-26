Dear ImGui, vendored for the StarFox_asteroid lab only.

Upstream:  https://github.com/ocornut/imgui
Branch:    docking
Version:   1.93.0 WIP
Commit:    fd13a1e8923a0a7077b404fc36fd063b25a0c0b5 (2026-08-19)

Only the core sources, the STB single-headers they inline, and the SDL2 +
OpenGL3 backends are kept - the examples, docs and other backends are not
vendored. Nothing here is patched; refreshing it is a straight re-copy of
those files from upstream.

Built into StarFox_asteroid alone (see src/CMakeLists.txt). StarFox_run,
StarFox_lib and StarFox_test do not compile or link any of it.
