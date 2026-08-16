# MathViz

**MathViz** is a high-performance, real-time mathematical visualization engine built in C++20 and OpenGL 4.6 (SPIR-V Shaders). It provides interactive 2D curve plotting, multi-layer composite graphing, 3D raymarched surface visualization, and fractal exploration with anti-aliased rendering, customizable neon bloom, and video recording capabilities.

| Newton's Method Fractal | Mandelbrot Set Explorer |
| :---: | :---: |
| ![Newton Fractal Scene](https://i.postimg.cc/sxwjB7s6/newton-fractal-4th-colorful-01.png) | ![Mandelbrot Scene](https://i.postimg.cc/L6bQH5jh/mandelbrot-scene.png) |
| 2D plotting example | Atomic Orbital |
| ![2D Plot](https://i.postimg.cc/rsP6XJ8W/plot2d-glow-01.png) | ![Atomic Orbital](https://i.postimg.cc/W4BhjG1Q/atom-scene-3-1-0.png) |



## Building & Prerequisites

### Prerequisites

Ensure you have the following packages installed:

- **C++ Compiler**: GCC 13+ or Clang 16+ (C++20 support)
- **Build System**: `meson` (v1.0+) and `ninja`
- **Libraries**:
  - `glfw3` (v3.3+)
  - `glslangValidator` (for SPIR-V shader compilation)
  - `libavcodec`, `libavformat`, `libswscale`, `libavutil` (FFmpeg libraries)

#### On Arch Linux:
```bash
sudo pacman -S gcc meson ninja glfw-x11 glslang ffmpeg
```

#### On Ubuntu / Debian:
```bash
sudo apt update
sudo apt install build-essential meson ninja-build libglfw3-dev glslang-tools \
                 libavcodec-dev libavformat-dev libswscale-dev libavutil-dev
```

---

### Building the Project

1. **Setup the build directory**:
   ```bash
   meson setup build
   ```

2. **Compile**:
   ```bash
   ninja -C build
   ```

3. **Run**:
   ```bash
   ./build/mathviz
   ```

