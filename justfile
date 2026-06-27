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
