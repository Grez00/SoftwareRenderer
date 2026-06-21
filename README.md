# SoftwareRenderer

This is, as you may have guessed, a simple software rendering library. Written in C++. It supports:
- Rasterizing of lines and triangles
- Perspective-correct interpolation
- Texture loading and sampling
- Depth buffering
- Mesh loading from .obj files
- Scene loading from .json files
- Blinn-Phong lighting
- Positionable camera
- Homogenous clipping
- Back-face culling
- Frustum culling

![Cube](https://github.com/Grez00/SoftwareRenderer/blob/main/media/cube.gif)

The library has only one external dependency, *stb_image*, which it requires for loading images (for textures). The final output of the library is a FrameBuffer containing all of the pixel colors and depth values for a single frame. You can then use whatever other library you want to output these pixels to the screen. The demo file, *main.cpp*, uses SDL3.

Should you want to use the library yourself, a Makefile which builds it from source is provided.
