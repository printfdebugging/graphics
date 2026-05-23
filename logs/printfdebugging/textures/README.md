<!-- mtoc-start -->

* [Textures](#textures)
  * [Texture Coordinates](#texture-coordinates)
  * [Texture object creation](#texture-object-creation)
  * [Using texture objects](#using-texture-objects)
  * [Texture unit internal representation](#texture-unit-internal-representation)

<!-- mtoc-end -->

# Textures

## Texture Coordinates

These are part of the mesh's attributes i.e. we receive them in the vertex
shader as `in` variables and then pass them to the fragment shader. These help
us map vertices to positions on the texture with (0,0) being the bottom left
corner and (1,1) being the top right. Only the vertices are mapped and rest
all is interpolated by the shader.

## Texture object creation

These are the function calls we make to create an OpenGL texture object.
`glGenTextures` creates a texture object internally on the OpenGL side and
returns an integer (a key in the internal hash map). The man page says
"glGenTextures - generate texture names" and never once says that it creates
textures, but looking at the [source](https://gitlab.freedesktop.org/mesa/mesa/-/blob/main/src/mesa/main/texobj.c#L1215) clarified that.
```c
glGenTextures(1, &texture->texture);
/* since we didn't make any texture unit active, the current texture unit is used which is 0 by default. */
glBindTexture(GL_TEXTURE_2D, texture->texture);
glTexImage2D(GL_TEXTURE_2D, GL_ZERO, internal_format, width, height, GL_ZERO, format, data_type, data);
```

`glTexImage2D` gets the current texture unit's target texture, target in this
case is `GL_TEXTURE_2D` and stores the data there... (i didn't look into the
implementation of that).

## Using texture objects

Texture units are slots for textures, internally it's an array in the driver
code. glActiveTexture selects one of these, the idea is that each of these
can hold a texture.

Each texture unit can hold a texture of a type, these types are called
`target` in the manpage/function_signature or texture_index in the code (mesa
implementation).
```c
typedef enum
{
   TEXTURE_2D_MULTISAMPLE_INDEX,
   TEXTURE_2D_MULTISAMPLE_ARRAY_INDEX,
   TEXTURE_CUBE_ARRAY_INDEX,
   TEXTURE_BUFFER_INDEX,
   TEXTURE_2D_ARRAY_INDEX,
   TEXTURE_1D_ARRAY_INDEX,
   TEXTURE_EXTERNAL_INDEX,
   TEXTURE_CUBE_INDEX,
   TEXTURE_3D_INDEX,
   TEXTURE_RECT_INDEX,
   TEXTURE_2D_INDEX,
   TEXTURE_1D_INDEX,
   NUM_TEXTURE_TARGETS
} gl_texture_index;
```
Internally `glActiveTexture` sets the current texture unit and then
glBindTexture "gets the texture" internally and binds that to the
current texture unit i.e. sets the pointer of the current texture unit's
`GL_TEXTURE_2D` slot to the texture object.
```c
glActiveTexture(GL_TEXTURE0);
glBindTexture(GL_TEXTURE_2D, texture);
```

## Texture unit internal representation

```c
/**
 * Shader stages.
 *
 * For vertex/tessallation/geometry/fragment shaders:
 * The order must match how shaders are ordered in the pipeline.
 * The GLSL linker assumes that if i<j, then the j-th shader is
 * executed later than the i-th shader.
 */
typedef enum mesa_shader_stage {
   MESA_SHADER_NONE = -1,
   MESA_SHADER_VERTEX = 0,
   MESA_SHADER_TESS_CTRL = 1,
   MESA_SHADER_TESS_EVAL = 2,
   MESA_SHADER_GEOMETRY = 3,
   MESA_SHADER_FRAGMENT = 4,
   MESA_SHADER_COMPUTE = 5,
   MESA_SHADER_TASK = 6,
   MESA_SHADER_MESH = 7,

   /* Vulkan-only stages. */
   MESA_SHADER_RAYGEN       = 8,
   MESA_SHADER_ANY_HIT      = 9,
   MESA_SHADER_CLOSEST_HIT  = 10,
   MESA_SHADER_MISS         = 11,
   MESA_SHADER_INTERSECTION = 12,
   MESA_SHADER_CALLABLE     = 13,

   /* must be last so it doesn't affect the GL pipeline */
   MESA_SHADER_KERNEL = 14,
} mesa_shader_stage;


/**
 * GL related stages with mesh shader (not including CL)
 */
#define MESA_SHADER_MESH_STAGES (MESA_SHADER_MESH + 1)

/**
 * Max number of texture image units.  Also determines number of texture
 * samplers in shaders.
 */
#define MAX_TEXTURE_IMAGE_UNITS 32

#define MAX_COMBINED_TEXTURE_IMAGE_UNITS (MAX_TEXTURE_IMAGE_UNITS * MESA_SHADER_MESH_STAGES)

struct gl_texture_attrib
{
   ...
   struct gl_texture_unit Unit[MAX_COMBINED_TEXTURE_IMAGE_UNITS];
   ...
};

struct gl_context
{
...

   struct gl_texture_attrib	Texture;	/**< Texture attributes */
...
};

```

Why is `Unit` array `MAX_TEXTURE_IMAGE_UNITS * MESA_SHADER_MESH_STAGES` long?
`MAX_TEXTURE_IMAGE_UNITS` makes sense but not `MESA_SHADER_MESH_STAGES`.

