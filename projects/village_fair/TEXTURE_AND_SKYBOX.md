# Texture Mapping & Skybox Implementation
### Village Fair — OpenGL 3.3 Core Profile

---

## Table of Contents

1. [Project Shader Overview](#1-project-shader-overview)
2. [Texture Mapping — 2D Textures](#2-texture-mapping--2d-textures)
   - 2.1 [Vertex Layout & UV Coordinates](#21-vertex-layout--uv-coordinates)
   - 2.2 [The `load_texture()` Function](#22-the-load_texture-function)
   - 2.3 [Texture Registry](#23-texture-registry)
   - 2.4 [Texture Parameters (Wrapping & Filtering)](#24-texture-parameters-wrapping--filtering)
   - 2.5 [Binding Textures During Draw Calls](#25-binding-textures-during-draw-calls)
   - 2.6 [Texture Toggle (`textureOn`)](#26-texture-toggle-textureon)
   - 2.7 [Vertex Shader — UV Passthrough](#27-vertex-shader--uv-passthrough)
   - 2.8 [Fragment Shader — Texture × Lighting](#28-fragment-shader--texture--lighting)
3. [UV Tiling — The `extra` Parameter](#3-uv-tiling--the-extra-parameter)
4. [Special Cases — RGBA Alpha Textures](#4-special-cases--rgba-alpha-textures)
5. [Sphere Texture Mapping (`SphereWithTexture`)](#5-sphere-texture-mapping-spherewithtexture)
6. [Skybox Implementation](#6-skybox-implementation)
   - 6.1 [What is a Skybox?](#61-what-is-a-skybox)
   - 6.2 [Skybox Cube Geometry](#62-skybox-cube-geometry)
   - 6.3 [Cubemap Texture Loading — Day](#63-cubemap-texture-loading--day)
   - 6.4 [Cubemap Texture Loading — Night](#64-cubemap-texture-loading--night)
   - 6.5 [Skybox Shaders](#65-skybox-shaders)
   - 6.6 [Skybox Render Pass (Render Loop)](#66-skybox-render-pass-render-loop)
   - 6.7 [Day/Night Toggle](#67-daynight-toggle)
7. [The Critical Depth Trick — Why `GL_LEQUAL`](#7-the-critical-depth-trick--why-gl_lequal)
8. [Translation-Stripped View Matrix](#8-translation-stripped-view-matrix)
9. [Common Issues & Fixes](#9-common-issues--fixes)

---

## 1. Project Shader Overview

The project uses **three separate shader programs**:

| Shader Program | Vertex Shader | Fragment Shader | Purpose |
|---|---|---|---|
| `ourShader` | `vertexShader.vs` | `fragmentShader.fs` | All scene objects (textured + lit) |
| `lightCubeShader` | `lightVertexShader.vs` | `lightFragmentShader.fs` | Light source visualisation spheres |
| `skyboxShader` | `skybox.vert` | `skybox.frag` | Skybox cube map rendering |

---

## 2. Texture Mapping — 2D Textures

### 2.1 Vertex Layout & UV Coordinates

Every cube face is defined with **8 floats per vertex** packed into a single interleaved VBO:

```
[ x, y, z,   nx, ny, nz,   u, v ]
  ←position→  ←─normal─→  ←UV→
  offset 0     offset 12   offset 24
```

The full cube vertex buffer (`cube_vertices[]`) defines **24 unique vertices** (4 per face × 6 faces). Each face has independent UV coordinates so every face can be textured correctly — even though they share edge positions, they cannot share vertices because the UV coordinates differ per face.

```cpp
// Example — Back face (normal pointing -Z)
0.0f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  TXmax, TYmin,
0.5f, 0.0f, 0.0f,  0.0f, 0.0f, -1.0f,  TXmin, TYmin,
0.5f, 0.5f, 0.0f,  0.0f, 0.0f, -1.0f,  TXmin, TYmax,
0.0f, 0.5f, 0.0f,  0.0f, 0.0f, -1.0f,  TXmax, TYmax,
```

The three **VAO attribute pointers** that wire this layout to the shaders:

```cpp
// location = 0  →  position (3 floats, stride 8, offset 0)
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);

// location = 1  →  normal (3 floats, stride 8, offset 12 bytes)
glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)12);

// location = 2  →  UV (2 floats, stride 8, offset 24 bytes)
glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)24);
```

### 2.2 The `load_texture()` Function

All 2D textures are loaded through a centralised helper defined in `src/draw_static.h`:

```cpp
void load_texture(unsigned int& texture, string image_name, GLenum format)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Wrapping — repeat in both S and T axes
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Filtering — bilinear
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Load image with stb_image (flip vertically for OpenGL convention)
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(image_name.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        // Auto-detect actual format (RGBA if 4 channels, RGB otherwise)
        GLenum actualFormat = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, actualFormat, width, height,
                     0, actualFormat, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    stbi_image_free(data);
}
```

Key design decisions:
- `stbi_set_flip_vertically_on_load(true)` — stb_image loads images top-to-bottom; OpenGL expects bottom-to-top. This flip corrects the orientation.
- `glGenerateMipmap` — automatically generates the full mipmap chain for better quality at distance.
- The `format` parameter passed in (e.g. `GL_RGBA`) is used as a *hint*, but the actual channel count from stb_image overrides it for the internal/source format.

### 2.3 Texture Registry

All textures are loaded once at startup in `main()`:

| Variable | File | Format | Purpose |
|---|---|---|---|
| `texture0` | `transparent.png` | GL_RGBA | Invisible/no-texture placeholder |
| `texture1` | `grass_2.png` | GL_RGBA | Ground, large grass plane |
| `texture2` | `brickwall_1.jpg` | GL_RGB | Brick walls |
| `texture3` | `wallside1.png` | GL_RGBA | Wall side panels |
| `texture4` | `road1.png` | GL_RGBA | Road/path surface |
| `texture5` | `concrete1.png` | GL_RGBA | Concrete surface 1 |
| `texture6` | `concrete2.png` | GL_RGBA | Concrete surface 2 |
| `texture7` | `concrete3.png` | GL_RGBA | Concrete surface 3 |
| `texture8` | `ballprint.png` | GL_RGBA | Ball print pattern (sport balls) |
| `texture10` | `leather2.png` | GL_RGBA | Leather surface |
| `garden1` | `flower2.png` | GL_RGBA | Garden flowers type 1 |
| `garden2` | `flower1.png` | GL_RGBA | Garden flowers type 2 |
| `whiteflower` | `white.png` | GL_RGBA | White flower / glass case |
| `shoptex` | `shop.png` | GL_RGBA | Shop sign / facade |
| `bambooTex` | `bamboo.png` | GL_RGBA | Bamboo/stall walls |

> **Note:** `texture9` is declared but not loaded — it is reserved for future use.

### 2.4 Texture Parameters (Wrapping & Filtering)

All 2D textures use the same parameters set inside `load_texture()`:

| Parameter | Value | Effect |
|---|---|---|
| `GL_TEXTURE_WRAP_S` | `GL_REPEAT` | Texture tiles horizontally when UV > 1 |
| `GL_TEXTURE_WRAP_T` | `GL_REPEAT` | Texture tiles vertically when UV > 1 |
| `GL_TEXTURE_MIN_FILTER` | `GL_LINEAR` | Bilinear filtering when texture is minified |
| `GL_TEXTURE_MAG_FILTER` | `GL_LINEAR` | Bilinear filtering when texture is magnified |

Mipmaps are generated (`glGenerateMipmap`) but the min filter is `GL_LINEAR` not `GL_LINEAR_MIPMAP_LINEAR`, so mipmaps are not actively used during sampling — they exist but don't contribute to the final render.

### 2.5 Binding Textures During Draw Calls

Before drawing any textured object, the appropriate texture is bound to `GL_TEXTURE0`:

```cpp
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texture1);   // e.g. grass for ground
// ... set model matrix, material uniforms ...
glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
```

Inside the Shop and ShopVariant draw helpers, textures are switched mid-draw via a local lambda:

```cpp
auto drawBox = [&](glm::vec3 pos, glm::vec3 size, glm::vec4 col, unsigned int tex, ...) {
    glBindTexture(GL_TEXTURE_2D, tex);   // ← per-part texture bind
    // set uniforms and draw...
};
```

This lets a single complex object (e.g., a shop stall) have different textures on different parts — bamboo walls, wooden roof, glass case — in a single function call.

### 2.6 Texture Toggle (`textureOn`)

A global boolean `textureOn` (default `false`) controls whether the bamboo/shop texture or a blank transparent texture is used:

```cpp
Shop(ourShader, translateMatrix * rotCW,
     glm::vec4(1,1,1,1),
     textureOn ? texture0 : bambooTex);   // transparent vs. bamboo
```

When `textureOn == false` — objects display with full textures applied.  
When `textureOn == true` — objects use `texture0` (transparent), so only the Phong-shaded material color is visible.

> This naming is counter-intuitive: `false` = textures on, `true` = textures off (plain color mode).

### 2.7 Vertex Shader — UV Passthrough

`shaders/vertexShader.vs` receives UV coordinates and passes them straight through to the fragment stage without modification:

```glsl
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;   // ← UV in

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoord;                          // ← UV out

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    FragPos     = vec3(model * vec4(aPos, 1.0f));
    TexCoord    = aTexCoord;                // passthrough — no transform
    Normal      = mat3(transpose(inverse(model))) * aNormal;
}
```

The normal is transformed with the **normal matrix** (`transpose(inverse(model))`) to handle non-uniform scaling correctly, ensuring lighting is accurate even when objects are stretched.

### 2.8 Fragment Shader — Texture × Lighting

`shaders/fragmentShader.fs` combines the sampled texture colour with the full Phong lighting result:

```glsl
uniform sampler2D ourTexture;
in vec2 TexCoord;

void main()
{
    vec4 dirL   = CalcDirLight(material, directionalLight, Normal, FragPos);
    vec4 pointL = vec4(0.0f);
    for(int i = 0; i < noOfPointLights; i++)
        pointL += CalcPointLight(material, pointLights[i], Normal, FragPos);
    vec4 spotL  = CalcSpotLight(material, spotLight, Normal, FragPos);

    vec4 result = dirL + pointL + spotL;

    if(!lightingOn)
        result = material.ambient;           // flat shading when lighting is off

    FragColor = texture(ourTexture, TexCoord) * result;  // ← texture × lighting
}
```

The final colour is **multiplicative**: the texture sample acts as a colour filter on top of the Phong result. When `texture0` (transparent/white) is bound, the texture sample returns `(1,1,1,1)`, so only the material lighting is visible.

The fragment shader supports **3 light types** simultaneously:
- **Directional light** — `CalcDirLight()` — sun/global
- **4 Point lights** — `CalcPointLight()` loop — lamp posts
- **1 Spot light** — `CalcSpotLight()` — flashlight-style

---

## 3. UV Tiling — The `extra` Parameter

Large surfaces (ground, road) need texture tiling or they look blurry and stretched. The UV coordinates are scaled by a global `extra` value:

```cpp
float extra = 4.0f;
float TXmin = 0.0f;
float TXmax = 1.0f + extra;   // = 5.0
float TYmin = 0.0f;
float TYmax = 1.0f + extra;   // = 5.0
```

Because `GL_TEXTURE_WRAP_S/T = GL_REPEAT`, UV values above 1.0 simply repeat the texture. With `TXmax = 5.0f`, a single cube face tiles the texture **5 times** across its width and height.

All six cube faces share these same `TXmin/TXmax/TYmin/TYmax` constants, meaning every face tiles at the same rate. To tile differently per face you would define separate UV constants.

---

## 4. Special Cases — RGBA Alpha Textures

Several textures use the `GL_RGBA` format with a genuine alpha channel:

- `texture0` (`transparent.png`) — fully transparent placeholder
- `bambooTex` (`bamboo.png`) — bamboo with transparent gaps
- `whiteflower` (`white.png`) — glass case effect

For alpha blending to work, `GL_BLEND` must be enabled. Currently the project does **not** enable `glEnable(GL_BLEND)` globally, which means transparent pixels on RGBA textures render as black or opaque depending on driver behaviour. This is a known limitation — the transparncy works partially depending on the PNG's content.

---

## 5. Sphere Texture Mapping (`SphereWithTexture`)

The `ball1` object is an instance of `SphereWithTexture`, a dedicated class in `include/sphereWithTexture.h`. It generates its own UV coordinates during sphere tessellation using the standard spherical mapping formula:

```
u = longitude / (2π)   →  [0, 1] wrapping around equator
v = latitude  / π      →  [0, 1] from pole to pole
```

This class maintains its own VAO/VBO/EBO separate from `cubeVAO` and uses `texture8` (`ballprint.png`) for both the ambient and diffuse texture slots.

---

## 6. Skybox Implementation

### 6.1 What is a Skybox?

A **skybox** is an infinitely-large cube surrounding the entire scene. The inside faces of the cube are textured with a panoramic photo, creating the illusion of a real-world environment (sky, landscape, horizon). The trick is that the skybox is always rendered at the camera's position — it never translates — so it always appears at infinite distance.

### 6.2 Skybox Cube Geometry

The skybox uses a **unit cube** centred at the origin, defined with 8 vertices and 12 triangles (6 faces × 2 triangles):

```cpp
float skyboxVertices[] = {
    -1.0f, -1.0f,  1.0f,   // 0  front-bottom-left
     1.0f, -1.0f,  1.0f,   // 1  front-bottom-right
     1.0f, -1.0f, -1.0f,   // 2  back-bottom-right
    -1.0f, -1.0f, -1.0f,   // 3  back-bottom-left
    -1.0f,  1.0f,  1.0f,   // 4  front-top-left
     1.0f,  1.0f,  1.0f,   // 5  front-top-right
     1.0f,  1.0f, -1.0f,   // 6  back-top-right
    -1.0f,  1.0f, -1.0f    // 7  back-top-left
};

unsigned int skyboxIndices[] = {
    1, 2, 6,  6, 5, 1,     // Right  (+X)
    0, 4, 7,  7, 3, 0,     // Left   (-X)
    4, 5, 6,  6, 7, 4,     // Top    (+Y)
    0, 3, 2,  2, 1, 0,     // Bottom (-Y)
    0, 1, 5,  5, 4, 0,     // Back   (+Z / front face of cube)
    3, 7, 6,  6, 2, 3      // Front  (-Z / back face of cube)
};
```

This skybox uses an **EBO (Element Buffer Object)** rather than the more common non-indexed triangle strip. The VAO is set up with only position data (3 floats, no normals, no UVs) — the 3D position vector itself doubles as the cubemap texture coordinate.

```cpp
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
glEnableVertexAttribArray(0);
```

### 6.3 Cubemap Texture Loading — Day

A **cubemap** texture (`GL_TEXTURE_CUBE_MAP`) stores 6 faces loaded as separate image files. They map to the six sides of the invisible cube using `GL_TEXTURE_CUBE_MAP_POSITIVE_X + i`.

```cpp
string facesCubemapDay[6] = {
    "textures/back1.png",    // GL_TEXTURE_CUBE_MAP_POSITIVE_X  (+X / Right)
    "textures/left1.png",    // GL_TEXTURE_CUBE_MAP_NEGATIVE_X  (-X / Left)
    "textures/top1.png",     // GL_TEXTURE_CUBE_MAP_POSITIVE_Y  (+Y / Top)
    "textures/bottom1.png",  // GL_TEXTURE_CUBE_MAP_NEGATIVE_Y  (-Y / Bottom)
    "textures/right1.png",   // GL_TEXTURE_CUBE_MAP_POSITIVE_Z  (+Z / Front)
    "textures/left12.png"    // GL_TEXTURE_CUBE_MAP_NEGATIVE_Z  (-Z / Back)
};
```

> **Note:** The file names don't fully match the semantic face names — e.g. `back1.png` is loaded as the `+X` (Right) face. The visual result is what matters; the names reflect the image content rather than strict OpenGL convention.

The texture parameters for the cubemap:

```cpp
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
```

`GL_CLAMP_TO_EDGE` on all three axes (S, T, R) prevents seam artefacts at the edges where two cubemap faces meet.

Each face is loaded with:
```cpp
stbi_set_flip_vertically_on_load(false);   // ← cubemap faces must NOT be flipped
glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height,
             0, GL_RGBA, GL_UNSIGNED_BYTE, data);
glGenerateMipmap(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i);
```

> `stbi_set_flip_vertically_on_load(false)` is **critical** for cubemaps. Unlike 2D textures, cubemap faces must not be flipped vertically or the sky and ground will appear swapped.

### 6.4 Cubemap Texture Loading — Night

A second cubemap is loaded for the night sky using JPEG files:

```cpp
string facesCubemapNight[6] = {
    "textures/night_right.jpeg",
    "textures/night_left.jpeg",
    "textures/night_top.jpeg",
    "textures/night_bottom.jpeg",
    "textures/night_front.jpeg",
    "textures/night_back.jpeg"
};
```

This uses `GL_RGB` format (no alpha) since JPEG has no transparency. Both cubemaps (`cubemapTextureDay` and `cubemapTextureNight`) are loaded at startup and kept in GPU memory simultaneously.

### 6.5 Skybox Shaders

**`shaders/skybox.vert`**
```glsl
#version 330 core
layout (location = 0) in vec3 aPos;

out vec3 texCoords;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    vec4 pos = projection * view * vec4(aPos, 1.0f);
    gl_Position = vec4(pos.x, pos.y, pos.w, pos.w);  // ← depth trick
    texCoords = vec3(aPos.x, aPos.y, -aPos.z);        // ← Z flipped
}
```

Two important details:

1. **`gl_Position = vec4(pos.x, pos.y, pos.w, pos.w)`**  
   Setting z = w forces NDC depth to `z/w = 1.0` (the maximum depth value) so the skybox always passes the depth test against every scene object. This keeps the skybox behind everything without needing to draw it first.

2. **`texCoords = vec3(aPos.x, aPos.y, -aPos.z)`**  
   The Z axis is negated to account for OpenGL's right-handed coordinate system vs the cubemap's left-handed texture lookup convention. Without this flip, the left and right faces of the skybox appear mirrored.

**`shaders/skybox.frag`**
```glsl
#version 330 core
out vec4 FragColor;

in vec3 texCoords;

uniform samplerCube skybox;

void main()
{
    FragColor = texture(skybox, texCoords);
}
```

The cubemap sampler takes a **3D direction vector** (not 2D UV). The direction from the origin to each vertex position is used directly as the lookup vector, which is why the vertex position doubles as the texture coordinate.

### 6.6 Skybox Render Pass (Render Loop)

The skybox is drawn **after** all scene objects, not before. This is the opposite of the naive approach but is more efficient — pixels that will be covered by scene geometry can be early-rejected by the depth test, saving fragment shader work.

```cpp
// 1. Change depth test function to LESS-OR-EQUAL
glDepthFunc(GL_LEQUAL);

// 2. Switch to skybox shader
skyboxShader.use();

// 3. Strip translation from the view matrix (only rotation remains)
glm::mat4 view1      = glm::mat4(glm::mat3(camera.GetViewMatrix()));
glm::mat4 projection1 = glm::perspective(glm::radians(camera.Zoom),
                           (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

skyboxShader.setMat4("view",       view1);
skyboxShader.setMat4("projection", projection1);

// 4. Bind the correct cubemap and draw
glBindVertexArray(skyboxVAO);
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_CUBE_MAP, dark ? cubemapTextureNight : cubemapTextureDay);
glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
glBindVertexArray(0);

// 5. Restore default depth function
glDepthFunc(GL_LESS);
```

### 6.7 Day/Night Toggle

The global boolean `dark` (toggled with the `0` key) selects which cubemap is active:

```cpp
if (dark)
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureNight);
else
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTextureDay);
```

Switching night mode also toggles lights automatically (in `key_callback`):
- **Day → Night (`dark = true`):** All four point lights turn on, directional light turns off.
- **Night → Day (`dark = false`):** Point lights turn off, directional light turns on.

---

## 7. The Critical Depth Trick — Why `GL_LEQUAL`

The depth buffer starts each frame cleared to `1.0` (maximum depth). The skybox vertex shader forces every skybox fragment's depth to exactly `1.0` (by writing `z = w`, making NDC depth = `w/w = 1.0`).

With the default `GL_LESS` function, a fragment at depth **1.0** would fail the test against the clear value **1.0** (since 1.0 is not less than 1.0), and the skybox would be invisible.

Switching to **`GL_LEQUAL`** (less-than-or-equal) allows fragments at depth = 1.0 to pass, so the skybox renders only where no scene object has been drawn (depth still = 1.0 from the clear).

```
glDepthFunc(GL_LEQUAL);   // skybox renders where depth == 1.0 (empty space)
// ... draw skybox ...
glDepthFunc(GL_LESS);     // restore to default for next frame
```

---

## 8. Translation-Stripped View Matrix

For objects in the scene, the view matrix includes both **rotation** and **translation** — moving the camera moves the world.

For the skybox, we want it to appear infinitely distant — moving the camera should never reveal its edges. The solution is to strip the translation component from the view matrix, leaving only rotation:

```cpp
// Full view matrix (rotation + translation)
glm::mat4 fullView = camera.GetViewMatrix();

// Skybox view matrix — rotation only, no translation
glm::mat4 view1 = glm::mat4(glm::mat3(fullView));
//                           ↑ mat3 extracts top-left 3×3 (pure rotation)
//                 mat4(...)  promotes back to 4×4 with zero translation
```

This is the **single most important** technique for a correct skybox. Without it, the skybox would appear to move with the camera, and walking toward a skybox edge would reveal it is a finite cube.

---

## 9. Common Issues & Fixes

| Symptom | Cause | Fix |
|---|---|---|
| Skybox invisible / black | `GL_DEPTH_FUNC` is `GL_LESS`; depth 1.0 fails | Use `glDepthFunc(GL_LEQUAL)` before drawing skybox |
| Skybox moves with camera | Translation not stripped from view matrix | Use `glm::mat4(glm::mat3(view))` for skybox |
| Sky/ground upside down | Cubemap loaded with `stbi_set_flip_vertically_on_load(true)` | Set flip to `false` for cubemap faces |
| Seams visible at skybox edges | Wrap mode not clamped | Set `GL_CLAMP_TO_EDGE` for S, T, and R axes |
| Textures appear mirrored horizontally | Z not negated in skybox vertex shader | Use `texCoords = vec3(aPos.x, aPos.y, -aPos.z)` |
| Objects show black where texture is transparent | Blending not enabled | Add `glEnable(GL_BLEND); glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA)` |
| Ground appears in only one direction | Cube vertices span 0→0.5; scale offsets wrong | Translate center = `target - (scale * 0.5 * 0.5)` to center the plane |
| Scene objects clipped at distance | Far clip plane too small | Increase from `100.0f` to `1000.0f` in `glm::perspective()` |

---

*Generated from source: `main.cpp`, `src/draw_static.h`, `src/callbacks.h`, `shaders/vertexShader.vs`, `shaders/fragmentShader.fs`, `shaders/skybox.vert`, `shaders/skybox.frag`*
