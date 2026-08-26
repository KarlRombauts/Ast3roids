# Fonts

## JetBrainsMono-Medium.ttf

JetBrains Mono, Medium weight, subset to printable ASCII (U+0020–U+007E) with
`pyftsubset` so the web build preloads 64 KB instead of 2 MB. Subset from the
locally installed Nerd Fonts build of the same typeface; the ASCII outlines are
JetBrains Mono's own, unpatched.

- Upstream: https://github.com/JetBrains/JetBrainsMono
- Copyright: The JetBrains Mono Project Authors
- Licence: SIL Open Font License 1.1

`OFL.txt` beside this file is upstream's licence text, as the OFL requires. It
lives in `Assets/` rather than the repository root deliberately: that directory
is preloaded into the WebAssembly bundle, so the licence travels inside the same
file the font itself ships in.

Regenerating the subset:

```sh
pyftsubset JetBrainsMono-Medium.ttf --unicodes=U+0020-007E \
    --output-file=src/Assets/Fonts/JetBrainsMono-Medium.ttf
```

The HUD (`src/Systems/HudSystem.cpp`) loads this through `Render/Font`, which
bakes a glyph atlas at the framebuffer's pixel size with stb_truetype. If the
file is missing the readouts fall back to `Render/VectorFont`.
