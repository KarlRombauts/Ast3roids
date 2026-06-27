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
