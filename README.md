# MathViz 🧮✨

**MathViz** is a high-performance, real-time mathematical visualization engine built in C++20 and OpenGL 4.6 (SPIR-V Shaders). It provides interactive 2D curve plotting, multi-layer composite graphing, 3D raymarched surface visualization, and fractal exploration with anti-aliased rendering, customizable neon bloom, and video recording capabilities.

![](https://postimg.cc/qNM4w7Jr)


## 🌟 Key Features

- **Multi-Layer 2D Procedural Plotter (`Plot2D`)**:
  - Layer multiple independent mathematical graph shaders on the same 2D coordinate plane with hardware alpha blending.
  - Subpixel anti-aliasing via distance-field rendering ($d = |y - f(x)| / \sqrt{1 + (f'(x))^2}$).
  - Multi-tier exponential neon bloom glow.
- **High-Speed Vertex Line Plotter (`LinePlot2D`)**:
  - Fast `GL_LINE_STRIP` rasterization using RAII-wrapped VBOs and VAOs (`LineMesh`).
  - Renders thousands of points with near-zero GPU fill-rate overhead.
- **3D Raymarched Surface Engine (`Plot3D`)**:
  - Real-time heightfield raymarching for 3D wave surfaces, drum membranes, and solitons.
  - Interactive orbit camera ($\text{yaw}, \text{pitch}, \text{distance}$).
  - Blinn-Phong specular shading, elevation colormaps, and glowing wireframe isolines.
- **Shared GLSL Math Library (`math_functions.glsl`)**:
  - Elementary & special functions (`sinc`, `sech`, `gaussian`, `lorentzian`, `sigmoid`, `softplus`).
  - Complex number arithmetic ($\mathbb{C}$ exp, log, sin, cos, power).
  - Anti-aliasing and implicit curve rendering helpers ($F(x, y) = 0$).
  - Fourier series wave synthesis (Square, Triangle, Sawtooth).
- **Fractal & Physics Demos**:
  - Mandelbrot & Julia sets, Newton's method fractals, 3D atomic orbital particles, and standing wave simulations.
- **MP4 Video Exporter**:
  - Offscreen/real-time frame encoding using native FFmpeg C API (`libavcodec`/`libavformat`).

---

## 🏗️ Architecture Overview

```
                          +-------------------+
                          |    App (GLFW)     |
                          +---------+---------+
                                    |
                                    v
                          +-------------------+
                          |    Scene (Base)   |
                          +---------+---------+
                                    |
                                    v
                          +-------------------+
                          |    Plot (Base)    |
                          +----+----+----+----+
                               |    |    |
        +----------------------+    |    +----------------------+
        |                           |                           |
        v                           v                           v
+---------------+           +---------------+           +---------------+
|    Plot2D     |           |  LinePlot2D   |           |    Plot3D     |
| (Procedural)  |           | (GL_LINE_STRIP|           | (Raymarched)  |
+---------------+           +---------------+           +---------------+
```

---

## 🎮 Interactive Controls

| Category | Key / Input | Action |
|---|---|---|
| **Navigation** | Mouse Drag | Pan 2D viewport or orbit 3D camera |
| **Navigation** | Scroll Wheel / `S` / `D` | Zoom in / Zoom out |
| **Toggles** | `V` | Toggle Neon Glow Bloom |
| **Toggles** | `B` | Toggle Coordinate Grid Lines |
| **Toggles** | `N` | Toggle Coordinate Axes |
| **Toggles** | `T` | Toggle Mouse Math Cursor Reticle |
| **Layers** | `1` – `5` | Toggle visibility of individual plot layers |
| **Tuning** | `Up` / `Down` | Increase / Decrease parameter $a$ |
| **Tuning** | `Left` / `Right` | Increase / Decrease parameter $b$ |
| **Tuning** | `[` / `]` | Increase / Decrease parameter $c$ |
| **System** | `R` | Reset camera view and scale |
| **System** | `P` | Save current screenshot (.ppm) |
| **System** | `Escape` | Exit application |

---

## 🛠️ Building & Prerequisites

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

---

## 📁 Repository Structure

```
MathViz/
├── include/              # Header files
│   ├── ogl/              # OpenGL wrappers (Shader, Program, App)
│   ├── scene/            # Scene hierarchy (Plot, Plot2D, LinePlot2D, Plot3D, etc.)
│   ├── utils/            # Utilities and RAII Buffer wrappers
│   └── writer/           # Video and image export writers
├── src/                  # C++ implementation files
│   ├── scene/            # Scene implementations
│   └── main.cpp          # Application entry point
├── res/shaders/          # GLSL Shaders & Libraries
│   ├── common_uniforms.glsl # Centralized GLSL uniform layout locations
│   ├── math_functions.glsl  # Shared mathematical & curve drawing functions
│   ├── colormap.glsl        # Perception-calibrated colormaps
│   └── plots/               # Modular plot fragment shaders
├── meson.build           # Meson build configuration
└── README.md             # Project documentation
```

---

## 📄 License

Distributed under the MIT License. See `LICENSE` for details.
