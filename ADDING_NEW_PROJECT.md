# Adding a New Project to the Build System

This document describes every step required to register a new project folder
inside `projects/` so it builds and runs like the existing ones.

---

## Project Directory Layout

```
D:\Documents\3D graphics\
├── CMakeLists.txt          ← root build config — THE only file you edit
├── build\                  ← cmake output (never edit)
├── opengl\
│   ├── glad.c              ← shared GLAD source (auto-linked by cmake)
│   └── include\
│       └── glad\, KHR\     ← shared GLAD + OpenGL headers
└── projects\
    ├── aeroplane\          ← example: simple project (uses add_opengl_project)
    ├── bulldozer\          ← example: simple project
    ├── curves\             ← example: project with root-level shaders
    ├── shelf\              ← example: simple project
    └── village_fair\       ← example: complex project (textures, image_loader, sub-dirs)
        ├── include\        ← project-specific headers
        ├── shaders\        ← project-specific shaders
        ├── textures\       ← project-specific textures
        ├── points\         ← project-specific data files
        └── src\            ← project-specific source splits
```

---

## Step 1 — Create the project folder

Place your new project under `projects\`:

```
projects\
└── my_project\
    ├── main.cpp            ← required entry point
    ├── shader.h            ← put headers here (they are auto-included)
    ├── camera.h
    ├── vertexShader.vs     ← shaders can live at root or in a shaders\ sub-folder
    ├── fragmentShader.fs
    └── shaders\            ← optional sub-folder, also auto-copied
        ├── scene.vs
        └── scene.fs
```

**Rules:**
- The **main source file** must be named `main.cpp` (or note the actual name — you will specify it in CMake).
- All **headers** (`.h`) placed in the project folder are automatically on the include path.
- Shaders at the **project root** (`.vs`, `.fs`) and inside a `shaders\` sub-folder are both **auto-copied** to the build output by the `add_opengl_project` function.
- `glad/glad.h` and `GLFW/glfw3.h` are already available globally — do **not** copy `glad.c` into your project.

---

## Step 2 — Add the project to `CMakeLists.txt`

Open `D:\Documents\3D graphics\CMakeLists.txt`.

### Case A — Simple project (no textures, no extra `.cpp` files)

This covers the majority of projects (`bulldozer`, `shelf`, `aeroplane`, `curves`).

Add **one line** near the other `add_opengl_project` calls (around line 72):

```cmake
add_opengl_project(my_project main.cpp)
```

That single call handles:
- Compiling `main.cpp` + `glad.c`
- Adding the project folder to the include path
- Linking OpenGL, GLFW, GLM
- Setting the output directory to `build/projects/my_project/`
- Copying all `.vs` and `.fs` shaders (root and `shaders\` sub-folder)

---

### Case B — Project with textures and/or extra `.cpp` files

This covers projects like `village_fair` and `codes`.  
You cannot use `add_opengl_project()` — write a **manual block** at the bottom of `CMakeLists.txt`:

```cmake
# -----------------------------------------------
# my_project — has textures and image_loader.cpp
# -----------------------------------------------
add_executable(my_project
    projects/my_project/main.cpp
    projects/my_project/image_loader.cpp   # ← only if you have a separate stb_image TU
    ${OPENGL_DIR}/glad.c
)

target_include_directories(my_project PRIVATE
    ${CMAKE_SOURCE_DIR}/projects/my_project
    ${CMAKE_SOURCE_DIR}/projects/my_project/include   # ← if you have an include\ sub-dir
)

target_link_libraries(my_project PRIVATE
    OpenGL::GL
    glfw
    glm::glm
)

set_target_properties(my_project PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/projects/my_project
)

# Copy shaders (from shaders\ sub-folder)
file(GLOB MP_SHADERS
    ${CMAKE_SOURCE_DIR}/projects/my_project/shaders/*.vs
    ${CMAKE_SOURCE_DIR}/projects/my_project/shaders/*.fs
    ${CMAKE_SOURCE_DIR}/projects/my_project/shaders/*.vert
    ${CMAKE_SOURCE_DIR}/projects/my_project/shaders/*.frag
)
foreach(SHADER ${MP_SHADERS})
    get_filename_component(SHADER_NAME ${SHADER} NAME)
    configure_file(${SHADER}
                  ${CMAKE_BINARY_DIR}/projects/my_project/shaders/${SHADER_NAME}
                  COPYONLY)
endforeach()

# Copy textures
file(GLOB MP_TEXTURES
    ${CMAKE_SOURCE_DIR}/projects/my_project/textures/*.png
    ${CMAKE_SOURCE_DIR}/projects/my_project/textures/*.jpg
    ${CMAKE_SOURCE_DIR}/projects/my_project/textures/*.jpeg
    ${CMAKE_SOURCE_DIR}/projects/my_project/textures/*.PNG
    ${CMAKE_SOURCE_DIR}/projects/my_project/textures/*.JPG
)
foreach(TEX ${MP_TEXTURES})
    get_filename_component(TEX_NAME ${TEX} NAME)
    configure_file(${TEX}
                  ${CMAKE_BINARY_DIR}/projects/my_project/textures/${TEX_NAME}
                  COPYONLY)
endforeach()

# Copy any data/points files
file(GLOB MP_DATA ${CMAKE_SOURCE_DIR}/projects/my_project/points/*.txt)
foreach(F ${MP_DATA})
    get_filename_component(F_NAME ${F} NAME)
    configure_file(${F}
                  ${CMAKE_BINARY_DIR}/projects/my_project/points/${F_NAME}
                  COPYONLY)
endforeach()

message(STATUS "Added project: my_project")
```

---

### Case C — Project that uses `stb_image`

`stb_image` must only be **defined once** per binary (the `STB_IMAGE_IMPLEMENTATION` macro).
The standard pattern used here is a dedicated `image_loader.cpp`:

**`projects/my_project/image_loader.cpp`** (create this file):
```cpp
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
```

Then in `main.cpp` (and any other `.cpp`), include without the define:
```cpp
#include "stb_image.h"   // no #define here
```

Add `image_loader.cpp` to the `add_executable()` source list (see Case B above).

> If your project is self-contained and defines `STB_IMAGE_IMPLEMENTATION` inside `main.cpp`
> directly (and has no other `.cpp` file), you do **not** need `image_loader.cpp` and can
> use `add_opengl_project(my_project main.cpp)` normally.

---

### Filenames with spaces

If any data file has spaces in its name (e.g. `points you can copy.txt`), use a
CMake variable to hold the path — **never** put the spaced path directly in `if(EXISTS ...)`:

```cmake
# WRONG — CMake splits on spaces:
if(EXISTS ${CMAKE_SOURCE_DIR}/projects/my_project/some file.txt)

# CORRECT — assign to variable first, then quote:
set(MY_FILE "${CMAKE_SOURCE_DIR}/projects/my_project/some file.txt")
if(EXISTS "${MY_FILE}")
    configure_file("${MY_FILE}" "${CMAKE_BINARY_DIR}/projects/my_project/some file.txt" COPYONLY)
endif()
```

---

## Step 3 — Reconfigure CMake

Run this **once** after editing `CMakeLists.txt` (or whenever you add new files to copy):

```powershell
cd "D:\Documents\3D graphics\build"
cmake ..
```

Check the output for your project name:
```
-- Added project: my_project
-- Configuring done
-- Generating done
```

> **You only need to re-run `cmake ..` when `CMakeLists.txt` itself changes.**
> Rebuilding source code changes does NOT require re-running cmake.

---

## Step 4 — Build and run

```powershell
cd "D:\Documents\3D graphics\build"
cmake --build . --target my_project
.\projects\my_project\my_project.exe
```

---

## Quick Reference — Which case applies to me?

| Condition | Use |
|---|---|
| Only `main.cpp`, shaders at root or in `shaders\`, no textures | **Case A** — `add_opengl_project(name main.cpp)` |
| Has a `textures\` folder | **Case B** — manual block with texture glob |
| Has an `include\` sub-folder for headers | **Case B** — add second `target_include_directories` entry |
| Has extra `.cpp` files (e.g. animation, mesh loaders) | **Case B** — list each `.cpp` in `add_executable` |
| Uses `stb_image` with a separate `image_loader.cpp` | **Case C** + **Case B** |
| Filenames with spaces anywhere | Use a CMake variable — see [Filenames with spaces](#filenames-with-spaces) |

---

## Existing project reference

| Project | CMake style | Notes |
|---|---|---|
| `bulldozer` | Case A | `add_opengl_project` |
| `shelf` | Case A | `add_opengl_project` |
| `aeroplane` | Case A | `add_opengl_project` |
| `curves` | Case B (manual) | Root-level shaders, spaced data filename |
| `codes` | Case B (manual) | `stb_image.cpp` separate TU, guarded by `if(EXISTS ...)` |
| `village_fair` | Case B (manual) | `image_loader.cpp`, `include\`, `shaders\`, `textures\`, `points\` |

---

## Common mistakes

| Mistake | Symptom | Fix |
|---|---|---|
| Forgot to run `cmake ..` after editing `CMakeLists.txt` | New target not found when building | Run `cmake ..` in `build\` |
| `#define STB_IMAGE_IMPLEMENTATION` in multiple `.cpp` files | Linker error: duplicate symbol `stbi_load` | Define it in exactly one `.cpp` (`image_loader.cpp`) |
| Texture/shader path is relative to source, not build | `Failed to load texture` at runtime | CMake copies files to build dir — use relative paths from exe, not source |
| Spaced filename in bare `if(EXISTS ...)` | `CMake Error: too many arguments` | Assign path to variable first, quote on use |
| Header in `include\` sub-folder not found | Compile error: `No such file` | Add `target_include_directories` for the `include\` path |
| `glad.c` added manually when using `add_opengl_project` | Duplicate symbol `glad_glGenTextures` etc. | `add_opengl_project` already links `glad.c` — don't add it again |
