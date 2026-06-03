# gltf model loading
- [ ] create a node type and a mesh type
- [ ] load node's transforms as the primitive's attributes might not be in ndc
- [ ] create their render_type functions in the renderer
- [ ] change model.c to render load them properly (just load the primitives for now)
- [ ] change render_model function to then render nodes -> meshes -> primitives -> render_primitive
- [ ] render the cylinder engine (just the primitives, no textures etc)

# core
## logger
- [ ] implement a logger and provide a runtime flag `-v` to log the application activity to the stdout/stderr (all as a macro)

## text rendering
- [ ] gap buffer for text and ascii encoding (for now)
- [ ] send glyph primitives to the shader and do the font rendering over there (use harfbuzz)

## documentation
- [ ] add proper doxygen comments over the structs, their attributes and functions once things start settling down

## declarative widget toolkit
- [ ] to-imagine-later

# shaders
- [ ] use include guards in shaders and organize the code, use one shader, don't create one for each model, unless necessary
- [ ] cache the uniform locations in the shader object, it's wasteful to call glGetUniformLocation more than once
- [ ] cache shaders, don't switch shaders if the shader_options match, store them globally, manage life through refcounting.

# ci
- [ ] setup github pages ci for testing changes, mostly compilation on various platforms
- [ ] write some tests, integrate a testing framework, or create your own
