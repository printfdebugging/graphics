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

<!-- mtoc-end -->

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
- gltf uses right-handed coordinate system, where opengl uses left-handed (gltf's x & z axes are flipped relative to opengl's x & z axes)
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
