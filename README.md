# SoftwareRenderer

This is, as you may have guessed, a simple software rendering library. Written in C++. It supports:
- Rasterizing of lines and triangles
- Perspective-correct interpolation
- Texture loading and sampling
- A depth buffer
- Mesh loading from .obj files
- Blinn-Phong lighting
- Positionable camera
- Back-face culling
- Frustum culling

![Cube](https://github.com/Grez00/SoftwareRenderer/blob/main/media/cube.gif)

The library has only one external dependency, *stb_image*, which it requires for loading images (for textures). The final output of the library is a FrameBuffer containing all of the pixel colors and depth values for a single frame. You can then use whatever other library you want to output these pixels to the screen. The demo file, *main.cpp*, uses SDL3.

To use the library yourself, place *lib/librenderer.a* in your own lib folder, and place everything within *include/renderer* into your include folder. Be sure to include the library when compiling. A Makefile which builds the library from source is also provided.
