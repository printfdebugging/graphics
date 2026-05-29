# shaders
- [ ] use include guards in shaders and organize the code (don't over engineer)
- [ ] store the uniform locations, call glGetUniformLocation once, do it in the model loading chapter

# core
- [x] introduce types in `core/defines.h`
- [x] fix sanitizer and compiler warnings
- [x] rename from learnopengl to graphics
- [ ] model loading
  - [ ] cylinder engine
    - [ ] load the [cylinder engine](https://github.com/KhronosGroup/glTF-Sample-Models/tree/main/2.0/2CylinderEngine)
    - `nitrix: That's a good one to make sure your node hierarchy (local/global matrices) are done correctly. Lots of parts.`
- [ ] implement a logger
  - discuss it again on #c, i think it's still necessary for the user to be able to see what's going on
  - but i have doubts, because implementing all those "if logging enabled" conditionals would hurt the performance
  - and if we don't ship them in release, then what's the point, for dev, we have renderdoc and gdb/vscode to debug things
- [ ] rope data structure for utf-8 string handling
- [ ] harfbuzz unicode (utf-8) font rendering on gpu
- [ ] add doxygen comments in the headers
- [ ] a fast/customizable/beautiful and declarative widget toolkit
- [ ] host on github pages, create example project to show all the widgets, like collabora's design-system
