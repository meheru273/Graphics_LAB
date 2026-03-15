# Village-Fair Project: Walkthrough

## What Was Done

### 1. Folder Rename & Reorganization
Renamed `Village-fair` → `village_fair` and organized 31 flat files into a clean structure:

```
projects/village_fair/
├── main.cpp              ← entry point (2918 lines)
├── image_loader.cpp      ← STB_IMAGE_IMPLEMENTATION
├── README.md
├── Image_detail
├── include/              ← 9 header files
│   ├── shader.h, camera.h, basic_camera.h
│   ├── directionalLight.h, pointLight.h, spotLight.h
│   ├── sphere.h, sphereWithTexture.h, stb_image.h
├── shaders/              ← 9 shader files
│   ├── vertexShader.vs, fragmentShader.fs
│   ├── vertexShaderTexture.vs, fragmentShaderTexture.fs
│   ├── vertexShaderWithTexture.vs
│   ├── lightVertexShader.vs, lightFragmentShader.fs
│   └── skybox.vert, skybox.frag
├── points/               ← 5 Bezier curve data files
│   ├── ferris_wheel_points.txt, boat_points.txt
│   ├── cylinder_points.txt, carousal_points.txt, head_points.txt
└── textures/             ← (download from Google Drive)
```

**Deleted:** 4 Visual Studio artifacts ([.sln](file:///D:/Documents/3D%20graphics/projects/Village-fair/Final%20Project.sln), [.vcxproj](file:///D:/Documents/3D%20graphics/projects/village_fair/Final%20Project.vcxproj), [.vcxproj.filters](file:///D:/Documents/3D%20graphics/projects/Village-fair/Final%20Project.vcxproj.filters), [.vcxproj.user](file:///D:/Documents/3D%20graphics/projects/Village-fair/Final%20Project.vcxproj.user))

### 2. Path Updates in [main.cpp](file:///D:/Documents/3D%20graphics/projects/aeroplane/main.cpp)
- Shader paths: `"vertexShader.vs"` → `"shaders/vertexShader.vs"` (all 3 Shader constructors)
- Texture paths: `"transparent.png"` → `"textures/transparent.png"` (all 14 load_texture calls)
- Point paths: `"ferris_wheel_points.txt"` → `"points/ferris_wheel_points.txt"` (all 4 read_file calls)
- Removed unused `#include <windows.h>`

### 3. CMake Integration
- Replaced broken `add_opengl_project(village_fare_project main.cpp)` with a full custom block
- Added [image_loader.cpp](file:///D:/Documents/3D%20graphics/projects/Village-fair/image_loader.cpp) to sources (for STB_IMAGE_IMPLEMENTATION)
- Added `include/` to `target_include_directories`
- Added copy rules for `shaders/`, `points/`, and `textures/` to build dir
- Wrapped missing `codes` project in `if(EXISTS)` guard

## Build Verification ✅

```
cmake ..          → Configuring done ✓
cmake --build . --target village_fair
  [1/4] image_loader.cpp.obj  ✓
  [2/4] glad.c.obj            ✓
  [3/4] main.cpp.obj          ✓
  [4/4] Linking village_fair.exe ✓
```

**Output:** [build/projects/village_fair/village_fair.exe](file:///d:/Documents/3D%20graphics/build/projects/village_fair/village_fair.exe)

## How to Run

```powershell
cd \"D:\\Documents\\3D graphics\\build\"
cmake --build . --target village_fair
.\\projects\\village_fair\\village_fair.exe
```

> [!IMPORTANT]
> **Textures are required!** Download from: https://drive.google.com/drive/folders/1V_Cm3i2QD3OqfDCKdzOi43JPdBSG7Rcx
> Place them in `projects/village_fair/textures/`, then re-run `cmake ..` to copy them to the build dir.
