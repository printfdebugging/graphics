<!-- mtoc-start -->

* [models](#models)
  * [why use models](#why-use-models)
  * [model loading](#model-loading)
  * [gltf models](#gltf-models)
    * [scenes](#scenes)
      * [nodes](#nodes)
    * [binary data storage](#binary-data-storage)
      * [buffers](#buffers)
      * [buffer views](#buffer-views)
      * [glb stored buffer](#glb-stored-buffer)
      * [accessors](#accessors)
        * [sparse accessors](#sparse-accessors)
        * [data alignment](#data-alignment)
        * [accessor bounds](#accessor-bounds)
    * [geometry](#geometry)
      * [meshes](#meshes)
        * [morph targets](#morph-targets)
      * [skins](#skins)
      * [instantiation](#instantiation)
    * [texture data](#texture-data)
      * [textures](#textures)
      * [images](#images)
      * [samplers](#samplers)
    * [materials](#materials)
      * [metalic-roughness material](#metalic-roughness-material)
      * [additional textures](#additional-textures)
      * [alpha coverage](#alpha-coverage)
      * [double sided](#double-sided)
      * [default material](#default-material)
      * [point and line materials](#point-and-line-materials)
    * [cameras](#cameras)
      * [view matrix](#view-matrix)
      * [projection matrices](#projection-matrices)
    * [animations](#animations)

<!-- mtoc-end -->

> [!NOTE]
> Such notes are hardly helpful (after you are done writing them) as it's
> always better to check the spec as that has the complete information, though
> taking short notes helps you know that you really understood what you read,
> so it's more of a reading-assist than notes for reference, unless you are
> experimenting with what you read. But in this case, the spec is pretty specific
> about things, so there isn't much room for experiemntation there.
> 
> See lighting notes, they have many pictures and implementation details which
> are useful for reference later on, that saves the context and various paths
> explored, helps stay organized as you see the whole context, how i reached here
> what was i trying to do in the first place etc..

# models
## why use models
- cube is boring
- manually defining complex shapes is hard and impractical
- we create them using 3d modeling tools, these automatically genearte the vertices, normals & uv coordinates
- technical details are hidden from the designers in the exported file
- we (programmers) parse these files (there are many formats), extract what we need, send them to opengl

## model loading
- we usually use libraries for this
- these libraries load the model data into internal structures from where we can access the properties
- a mesh is a sub part of the model, usually a model is not created in one piece, but has several meshes, like one for hand, for face, for the jacket etc
- a single mesh contains enough information to fully draw the part it represents, a model puts multiple such meshes together
- there's a recursive nature to model loading
- meshes often have a parent-child relationship, so when the parent translates, all the child meshes follow

## gltf models
- read the chapters 1,2,3 of the [gltf specification](https://registry.khronos.org/glTF/specs/2.0/glTF-2.0.pdf)
- it's just json and an accompanying binary
- entites are referenced with non-negative indices into arrays + offset + length
- gltf uses camelCase
- gltf uses right-handed coordinate system (same as opengl)
  - gltf's x & z axes are flipped relative to opengl's x & z axes
  - `-z` is still the front, be it in opengl or for gltf models
  - gltf models are rotated by 180 degrees such that their -z points outwards, i.e. they face us
  - while in the opengl world the front is into the screen, so we rotate these models to match that
- units of all linear distances are in meters, all angles are in radians, rotation is coutnerclockwise

### scenes
- an asset may contain zero or more scenes, defined as `scenes` array
- if `scenes` is empty, then treat it as a collection of individual entities
- `scenes.nodes` array contains root nodes and these shouldn't be in any `node.children` array
- different scenes may share root nodes
- `scene` (optional) points to the index of default scene to render

#### nodes
- `nodes` array define the objects to render, organized in a parent-child hierarchy (node hierarchy) using a node's `chidren` array
- a root node is a node without a parent
- any node may contain *local space* transform properties, either as a `matrix`, or individual `trs` properties
- `ts` are `vec3` while `r` is unit quanternion (xyzw, w is scaler)
- when targeted for animation, `matrix` property must not be present
- `gtm of a node` = `gtm of parent` * `ltm of the node itself` (g(l)tm = global (local) tranformation matrix)
- for root node ltm is gtm

### binary data storage
#### buffers
- `buffers` array has buffers
- a buffer is arbitrary data stored in binary blob which is usually a separate file from the `.gltf` json file
- all buffer data must use little endian byte order
- `byteLength` defines the size of a buffer
- `buffer.data` may have embedded data in base64 encoding and in this case `buffer.mediatype` should be `application/stream` or `application/gltf-buffer`

#### buffer views
- `bufferViews` array has buffer views, each view represents a contiguous segment of data in a buffer (byteOffset + byteLength)
- `bufferView.target` property defines if it's an *array buffer* (vertices, uv, colors, normals) or *element array buffer* (indices)
- when used for vertex attribute data, `byteStride` may be defined.
- `buffers` and `bufferViews` don't contain type information (objects (meshes, skins, animations) access these through accessors)

#### glb stored buffer
- a `.glb` file has json and (one buffer) binary chunk packed together
- the buffer (who's data is proivided in a `.glb` file) must be the first buffer of `buffers array` (search "buffers" in a .glb file) 
- this first buffer mustn't define the `uri` attribute. the next buffers (not the first one) should define it, or else it's UB
- the bin chunk only belongs to the first buffer object (see the bold part highlighting the first buffer, just before the binary)
```markdown
glTFÂ´Â©JSON{"asset":{"generator":"COLLADA2GLTF","version":"2.0"},"scene":0,"scen
es":[{"nodes":[81,0]}],"nodes":[{"children":[80,79,78,77,76,75,74,73,13,10,7,4
,1]},{"children":[3,2],"matrix":[1.0,0.0,0.0,0.0,0.0,1.0,0.0,0.0,0.0,0.0,1.0,0
.0,136.860107421875,-64.45372009277344,-36.179630279541019,1.0]},{"mesh":0,"ma
trix":[-1.0,8.979318677493353e-11,0.0,0.0,-8.979318677493353e-11,-1.0,0.0,0.0,
................................
................................
:[0.0,0.0,0.0],"name":"Material_18"}],"bufferViews":[{"buffer":0,"byteOffset":
1340232,"byteLength":454380,"target":34963},{"buffer":0,"byteOffset":0,"byteLe
ngth":1340232,"byteStride":12,"target":34962}],"buffers":[**{"byteLength":179461 2}**]}
4bBINÂºÂz?Â_Ã¿=Â¢Â'>(az?ÂªÃÃ«=Ã1>Ã¬i{?`sÃ=Â¯#>5^z?Ã³ÂÂ¾('>|Â}?ÂÂ
ÃÂ½Â¨Â¦Ã=Ãv}?Â Â±Â½ÂÂÃ¢=wÂx?ÃºÃ°Â¾Ã¸Ãº:>ÃÃ{?Ã¦
8Â¾$Ão?Â¿&Â³Â¾ÃÃº?Â´Ã¥P?lÂ²Ã¾>Ã^?IÂ8?Ã¦Y1?ÂDÂ»>ÃÃ DÂ½ÂÃ±m?Ã²{Â»>"8n?Ã
¦
8>ÃÃ{?Ã¯85>Â]>ÃÃuÂ¿Ã²wÂ·>ÂªÃÃ«=Â¿,mÂ¿Ã¯85>Ã¯85>0ÃwÂ¿ÂªÂÃµ=Ã¿@Ã>ÃÃe
?ÃÃz>T8R>Ã·Âr?Â
Â±=Ã¥Â¶Ã¥>Â·c?.Âª=?ÂÃ°+Â¿Â®Ã²?ÂIMÂ¿YÂE?EÃ±Â¿hÂH>Â¹Âm?Ã¯8Â¥Â¾Ã¨>Â¾Ã Â¹
c?,ÃÂÂ¾ÃÃÂ¿Â¾ÂÃºe?]Â¥ÃÂ¾Ã¨>Â¾HÃÂ¿Ã£Q2?=ÃÃ¡Â¾Âº Â¿h?#'Â¿(
................................
................................
```
#### accessors
- all binary data is stored in buffers and retreived by accessors
- it defines method of *retreiving* data as *typed arrays* from a *buffer view*
- defines *component type* (scaler, vec3 etc) and *data type* (signed byte, float, unsigned int etc)
  - signed 32-bit int components are not supported
  - `NaN`, `+-Infinite` must not be present
- *count* defines the number of elements
- *byteOffset* defines offset within bufferView
- vertex attribute data might have a `bufferView.byteStride`, other kinds of data (animation keyframes, vertex indices..) is tightly packed
- `accessors` array stores all accessors
- `size = (bytes per componentType) * (components per element) * count;`

##### sparse accessors
- often used when encoding morph targets (like diffs between two different animation states of a model)
- initializes an array of typed elements from data stored in bufferView, like the standard accessor
- if `accessor.bufferView` undefined, then initialized as an array of zeroes
- includes a `sparce` json object describing elements that are different from their initialization values
- `sparce` contains the following properties:
  - count: number of displaced elements (<= #baseElements)
  - indices: *location* & *componentType* of indices of values to be replaced (strictly increasing & <= #baseElements)
  - values: object for locations of displaced elements (which indices referrs to)

> [!NOTE]
> I still don't understand how the replacement happens here, maybe
> doing it myself would help.

##### data alignment
- `accessor.byteOffset` and `bufferView.byteOffset + accessor.byteOffset` should be multiples of accessor's `componentType`
- when `bufferView.byteStride` is undefined, it means items are tightly packed i.e. `stride == sizeof the element`
- **todo**: come back to this

##### accessor bounds
- `min` and `max` arrays in an accessor define the per component min and max values
- length of these arrays must be equal to the number of accessor components
- `accessor.normalized` flag has no effect on these properties
- for floats, min/max should be rounded to single percision floats before use
- these (min/max) are must have for animation input and vertex position attribute accessors, optional for the rest

### geometry
- any node may contain one mesh
- a mesh may be skinned using info provided in a referneced `skin` object
- a mesh may have morph targets (??joints etc??)

#### meshes
- a mesh is an array of *primitives* required for draw calls
- a primitive has *attributes* (object) which is one or more vertex attributes (key value pairs)
- POSITION, NORMAL, TANGENT, TEXCOORD_n, COLOR_n, JOINTS_n, WEIGHTS_n are the spec defined attributes
- custom attributes must start with an '_' and mustn't be unsigned int componentType
- an indexed primitive has an *indices* property
- attributes and indices all reference to accessors for data
- a primitive may define *material* (??what is a material exactly, seems covered later on??) and *mode* (how to draw, lines, triangles etc..)
- if *material* undefined, default material must be used.
- POSITION accessor must have min/max properties defined
- all attribute accessors for a given primitive must have the same count
- all index values (whenever defined) must be less than attribute accessor's *count*
- *indices* must be valid for the topology type used
- mesh geometry shouldn't contain degenerate lines or triangles (use one vertex more than once per primitive)
- vertices of a same triangle should have the same `tangent.w` value (or else considered undefined)
- `bitangent = cross(normal.xyz, tangent.xyz) * tangent.w`

##### morph targets
- defined by extending the mesh concept (is a morphable mesh)
- todo: come back to this

#### skins
- bones are for visualization, they don't exist otherwise, only joints do (??scrutinize this later??)
- todo: come back to this

#### instantiation
- mesh is instanciated by `node.mesh` property (with an index of the mesh and transforms trs/matrix)
- when instianciating using triangle-based topoligy, the determinant of the node's global transform defines the winding order of that primitive
  - winding order is counter-clockwise when determinant is positive and vice-versa
  - switching winding order enables mirroring of geometry (via negative transforms)
- for morph targets, `node.weights` must be used. if undefined, use `mesh.weights`
- in a node, mesh defines the *mesh* for a skin instance while *skin* defines the skin (index)

### texture data
#### textures
- stored in `textures` array of the asset, each contains a `source` (image index) and a `sampler` (sampler index)
- when *sampler* not defined, one with repeat wrapping and auto filtering must be used

#### images
- referred by the  textures and stored in the *images* array of the asset
- each image contains one of `uri`, `embedded data uri` or `bufferView` (with *mimeType* defined)
- image is stored vertically flipped i.e. (0,0) uv coordinate is at the top left

#### samplers
- stored in the `samplers` array of an asset, each specifies filtering and wrapping methods (using integer enums)
- textures dimensions should be in powers of 2
- *filtering* controles the magnification and minification of textures
  - magFilter (nearest or linear)
  - minFilter (nearest, linear, ... 4 complicated names ...)
- *wrapping* defines how to handle uv coordinates beyond the 1x1 range
  - repeat, mirrored-repeat & clamp-to-edge are the supported modes

### materials
- defined in the `materials` array of the asset
- gltf defines materials in *pbr representation*
- it uses the *metalic-roughness* material model

#### metalic-roughness material
- `material.pbrMetallicRoughness` has all the parameters i.e. base color, metalness, roughness
- for metals, base color is interpreted as reflectance value at normal incidence, for non-metals, it's the reflected diffuse color of the material.
- can be specified with textures or factors like `baseColorFactor` or `baseColorTexture`.
- when texture is not given, assume a texture with all values 1.0
- when both factor and texture are mentioned, texture values are scaled by the factor (multiplied)
- if vertex's color attribute is defined, then this also is a linear multiplier to the base color
- texture has an `index` property pointing to the index of the texture and an optional `texCoord` for the index of texture coordinates
- metalness and roughness are specified in the same texture `metallicRoughnessTexture`, *b* for metalness values & *g* channel for roughness

#### additional textures
- a few additional textures are also supported
- *normal* texture encodes xyz componets of a normal vector in tangent space. these don't contain the alpha channel
  - the texel values map as following `red:  [0.0, 1.0] -> x: [-1, 1]`, `green:  [0.0, 1.0] -> y: [-1, 1]` & `blue:  (0.5, 1.0] -> y: (0, 1]`
  - it may also contain a *scale* value which linearly scales x and y components
- *occlusion* texture indicates areas which receive less *indirect* lighting from ambient sources.
  - the *red* channel encodes occlusion value where 0 means fully occluded and 1 means not occluded, other channels don't affect anything
  - may contain a scaler `strength` value used to reduce occlusion `result = 1.0 + strength * (occlusionTexture - 1.0)`
- *emissive* texture controls the intensity of light being emitted by the material.

#### alpha coverage
- `alphaMode` defines how the alpha value from the 4th component of base color (in metallic-roughness material model) is interpreted
- can be one of OPAQUE, MASK, BLEND
- for MASK, `alphaCutoff` specifies the cutoff threshold, when `alpha >= threshold` it's rendered as fully opaque, otherwise as fully transparent

#### double sided
- `doubleSided` specifies whether material is double sided, when false, only front-facing triangles are rendered (back-face culling is enabled)
- the back face must have it's normals reversed before the lighting equation is evaluated

#### default material
- used when a mesh doesn't specify the material
- defined as a material with no properties specified, doesn't emit light, is black unless some lighting is present in the scene

#### point and line materials
- the spec doesn't define the size and style of non-triangular primitives, but suggests a convention for consistency
- points and lines should have widths of 1px in viewport space.
- points or lines with NORMAL and TANGENT attributes should be rendered with standard lighting including normal textures.
- points or lines with NORMAL but without TANGENT attributes should be rendered with standard lighting but ignoring any normal textures on the material.
- points or lines with no NORMAL attribute should be rendered without lighting and instead use the sum of the base color value (as defined above, multiplied by COLOR_0 when present) and the emissive value.

### cameras
- stored in the `cameras` array of the asset
- `camera.type` designates the type of projection matrix (orthographic or perspective)
- `camera.orthographic` / `camera.perspective` properties define the details
- `node.camera` instanciates a camera
- the camera object defines a projection matrix which transforms the coordinates from the view space to the clip space
- the node containing camera defines a view matrix which transforms the coordinates from the world space to the view space

#### view matrix
- the camera follows left handed coordinate system with the front pointing towards the -z axes (same as opengl's -z)
- view matrix is derived from the camera's containing node's global transform (with scaling ignored)
- if the global transform is identity, the location of camera is at origin

#### projection matrices
- projection can be perspective or orthograhpic
- perspective projection has *finite* and *infinite* (when *zfar* is undefined) subtypes.

### animations
- todo: come back to this
