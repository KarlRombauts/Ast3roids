# Build tasks for the StarFox / asteroids-3d game.
# Run `just` with no arguments to see available recipes.

build_dir := "build-local"
binary    := build_dir / "src" / "StarFox_run"

# Show available recipes
default:
    @just --list

# Configure the CMake build (run once, or after changing CMakeLists)
configure:
    cmake -S . -B {{build_dir}} -DCMAKE_BUILD_TYPE=Debug

# Compile the game (configures first if needed)
build: configure
    cmake --build {{build_dir}} --target StarFox_run -j

# Compile and run the game (assets are copied next to the binary)
run: build
    cd {{build_dir}}/src && ./StarFox_run

# Remove the build directory
clean:
    rm -rf {{build_dir}}

# Clean, reconfigure, and build from scratch
rebuild: clean build

# --- WebAssembly (Emscripten) ---
# Requires the emsdk environment: `source ~/emsdk/emsdk_env.sh`

emsdk_env := "~/emsdk/emsdk_env.sh"

# Build the full game to WebAssembly (output in build-web/src/StarFox_run.html)
wasm:
    source {{emsdk_env}} && emcmake cmake -S . -B build-web && cmake --build build-web -j

# Build the toolchain smoke test (standalone WebGL2 clear screen) to build-web-smoke/
wasm-smoke:
    source {{emsdk_env}} && mkdir -p build-web-smoke && \
        emcc web/smoke.cpp -o build-web-smoke/index.html \
        -sUSE_SDL=2 -sFULL_ES3 -sMAX_WEBGL_VERSION=2 -sMIN_WEBGL_VERSION=2

# Serve a built web directory locally at http://localhost:8000
# (the game is at /StarFox_run.html). Default serves the full game build.
serve dir="build-web/src":
    cd {{dir}} && python3 -m http.server 8000

# --- Deploy (GitHub Pages) ---

release_dir := "build-web-release"

# -O3 roughly halves both the wasm and the JS against a default build, so this
# is what gets published.
# Build the optimised WebAssembly bundle (output in build-web-release/src)
wasm-release:
    source {{emsdk_env}} && emcmake cmake -S . -B {{release_dir}} -DCMAKE_BUILD_TYPE=Release && \
        cmake --build {{release_dir}} --target StarFox_run -j

# The shell is renamed to index.html because that is what the manifest's
# start_url points at, and .nojekyll stops Pages from processing the bundle.
# Publish the optimised build to gh-pages (the live game)
deploy message="Deploy the current build": wasm-release
    #!/bin/bash
    set -euo pipefail
    out="{{release_dir}}/src"
    for file in StarFox_run.html StarFox_run.js StarFox_run.wasm StarFox_run.data; do
        test -f "$out/$file" || { echo "missing $out/$file - build first"; exit 1; }
    done

    tree="$(mktemp -d)/gh-pages"
    git fetch origin gh-pages
    git worktree add "$tree" -B gh-pages origin/gh-pages

    cp "$out/StarFox_run.html" "$tree/index.html"
    cp "$out/StarFox_run.js" "$out/StarFox_run.wasm" "$out/StarFox_run.data" "$tree/"
    cp web/manifest.json "$tree/"
    touch "$tree/.nojekyll"

    git -C "$tree" add -A
    git -C "$tree" status --short
    git -C "$tree" commit -m "{{message}}"
    git -C "$tree" push origin gh-pages
    git worktree remove "$tree"
