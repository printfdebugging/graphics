
<!-- mtoc-start -->

* [font rendering](#font-rendering)
  * [harfbuzz](#harfbuzz)
    * [links to the pages](#links-to-the-pages)
  * [getting started](#getting-started)
  * [terminology](#terminology)
  * [hb-gpu](#hb-gpu)
  * [manual reading](#manual-reading)
    * [what is text shaping & why do i need it?](#what-is-text-shaping--why-do-i-need-it)
    * [the harfbuzz object model](#the-harfbuzz-object-model)
    * [blob](#blob)
    * [buffers](#buffers)

<!-- mtoc-end -->
# font rendering
## harfbuzz
- these are the resources which seem quite useful at the first look
- [Getting started with HarfBuzz: HarfBuzz Manual](https://harfbuzz.github.io/getting-started.html)
- [Terminology: HarfBuzz Manual](https://harfbuzz.github.io/terminology.html)
- [A simple shaping example: HarfBuzz Manual](https://harfbuzz.github.io/a-simple-shaping-example.html)
- [hb-gpu: HarfBuzz Manual](https://harfbuzz.github.io/harfbuzz-hb-gpu.html)

### links to the pages
- [A simple shaping example: HarfBuzz Manual](https://harfbuzz.github.io/a-simple-shaping-example.html)
- [Setting buffer properties: HarfBuzz Manual](https://harfbuzz.github.io/setting-buffer-properties.html)
- [Customizing Unicode functions: HarfBuzz Manual](https://harfbuzz.github.io/customizing-unicode-functions.html)
- [Fonts, faces, and output: HarfBuzz Manual](https://harfbuzz.github.io/fonts-and-faces.html)
- [Fonts, faces, and output: HarfBuzz Manual](https://harfbuzz.github.io/fonts-and-faces.html#fonts-and-faces-objects)
- [Customizing font functions: HarfBuzz Manual](https://harfbuzz.github.io/fonts-and-faces-custom-functions.html)
- [Font objects and HarfBuzz's native OpenType implementation: HarfBuzz Manual](https://harfbuzz.github.io/fonts-and-faces-native-opentype.html)
- [Working with OpenType Variable Fonts: HarfBuzz Manual](https://harfbuzz.github.io/fonts-and-faces-variable.html)
- [Glyphs and rendering: HarfBuzz Manual](https://harfbuzz.github.io/glyphs-and-rendering.html)
- [Shaping and shape plans: HarfBuzz Manual](https://harfbuzz.github.io/shaping-and-shape-plans.html)
- [Shaping and shape plans: HarfBuzz Manual](https://harfbuzz.github.io/shaping-and-shape-plans.html#shaping-buffer-output)
- [OpenType features: HarfBuzz Manual](https://harfbuzz.github.io/shaping-opentype-features.html)
- [Shaper selection: HarfBuzz Manual](https://harfbuzz.github.io/shaping-shaper-selection.html)
- [Plans and caching: HarfBuzz Manual](https://harfbuzz.github.io/shaping-plans-and-caching.html)
- [Clusters: HarfBuzz Manual](https://harfbuzz.github.io/clusters.html)
- [Clusters: HarfBuzz Manual](https://harfbuzz.github.io/clusters.html#clusters-and-shaping)
- [Working with HarfBuzz clusters: HarfBuzz Manual](https://harfbuzz.github.io/working-with-harfbuzz-clusters.html)
- [A clustering example for levels 0 and 1: HarfBuzz Manual](https://harfbuzz.github.io/a-clustering-example-for-levels-0-and-1.html)
- [Reordering in levels 0 and 1: HarfBuzz Manual](https://harfbuzz.github.io/reordering-in-levels-0-and-1.html)
- [The distinction between levels 0 and 1: HarfBuzz Manual](https://harfbuzz.github.io/the-distinction-between-levels-0-and-1.html)
- [Level 2: HarfBuzz Manual](https://harfbuzz.github.io/level-2.html)
- [Level 2: HarfBuzz Manual](https://harfbuzz.github.io/level-2.html#ligatures-with-combining-marks-in-level-2)
- [Level 2: HarfBuzz Manual](https://harfbuzz.github.io/level-2.html#reordering-in-level-2)
- [Level 2: HarfBuzz Manual](https://harfbuzz.github.io/level-2.html#other-considerations-in-level-2)
- [Common data types and APIs: HarfBuzz Manual](https://harfbuzz.github.io/utilities-common-types-apis.html)
- [hb-gpu: HarfBuzz Manual](https://harfbuzz.github.io/harfbuzz-hb-gpu.html)

## getting started
- harfbuzz is a text shaping library, hb_shape() does most of the work
- we provide a *font*, a buffer of *unicode codepoints* & some optional *font options*
- we get corresponding glyphs from the font, correctly ordered and positioned

## terminology
- script
  - it's a writing system
  - a set of symbols, rules, conventions used to represent a language (or multiple languages)
  - a script is not the same thing as a language
- shaper
  - it's a handler for a specific script-shaping model
  - there are quite a few of them
- cluster
  - sequence of codepoints treated as an indivisible unit
  - can include code point sequences from a ligature or base and mark sequences..
  - shaping operations might separate or reorder codepoints, so it's important to track and preserve clusters
- grapheme
  - one of the indivisible units that makes up a writing system or script
  - individual symbols (letters, numbers, punctuation marks logograms etc)
  - sometimes a grapheme might correspond to a sequence of codepoints
  - logograms are quite interesting indeed :)
- syllable
  - a sequence of sounds that makes up the building blocks of a particular language
  - this is important as re-ordering operations are performed at the syllable level
  - a syllable might contain one or more code points, depends on the writing system
  - note that syllables and shaping clusters might overlap but they are not necessarliy the same

## hb-gpu
- it mentions two types of rendering, draw and paint, the functions in the ref page are also namespaced *hb_gpu_draw* and *hb_gpu_paint*
- minth from ##OpenGL -> draw is monochrome, paint is color.
- i got confused by the hb-gpu documentation, so am looking into st's source (i use suckless terminal, have my own fork of it, it uses harfbuzz).
- hopefully i will get some idea of what goes around in font rendering and then i can come back and make sense out of the tutorial
- [Why do I need a shaping engine?: HarfBuzz Manual](https://harfbuzz.github.io/why-do-i-need-a-shaping-engine.html) explains the missing bits.
- [HarfBuzz Manual: HarfBuzz Manual](https://harfbuzz.github.io/) is the actual user manual index.
- the manual is quite rich, i should probably read most of it -> a multi-week endaevour indeed.

## manual reading
- manual index: [HarfBuzz Manual: HarfBuzz Manual](https://harfbuzz.github.io/)

### what is text shaping & why do i need it?
- process of translating unicode codepoints into a properly arranged sequence of glyphs that can be rendered properly on the screen/document
  - fonts contain a set of glyphs
  - a glyph is a shape which represents a letter or a number or a punctuation mark etc
  - glyph is indexed with a glyph id
  - a glyph id doesn't necessarily co-relate with a unicode codepoint
  - cmap table inside the font maps the unicode codepoints to glyph ids
- ligatures are combinations of characters rendered as a single unit
  - whether a combination will be rendered as a ligature or separate characters depends on the font (if it has that ligature glyph).. not the input text
  - text shaping involves querying the font's ligature tables and determining what substitutions should be made.
  - this might not be optional in some languages, like devanagri and tamil
  - text shaping tells you whether you have a precomposed glyph within your font or if you need to compose a glyph yourself out of combining marks—and, if so, where to position those marks.
  - [What HarfBuzz doesn't do: HarfBuzz Manual](https://harfbuzz.github.io/what-harfbuzz-doesnt-do.html) is a really important read.
- correctly shaping a text run in any script may still involve unicode normalization, ligature substitutions, mark positioning, kerning, and applying other font features.
- [Shaping operations: HarfBuzz Manual](https://harfbuzz.github.io/shaping-operations.html)
  - for shaping purposes, all real-world text streams must be first segmented into runs that have a uniform set of properties.
  - in particular, shaping models always assume that every codepoint in a text run has the same direction, script tag, and language tag.

### the harfbuzz object model
- it has *two kinds* of *data types*, *non-opaque*, pass by value types and *opaque*, heap-allocated types.
- *value types* expose the structs, only done where it's exteremely inefficient to do otherwise, eg: hb_glyph_info_t, hb_glyph_position_t
- *opaque types*, also called objects are ref-counted, have various create() methods, referenced with reference() and dereferenced with destroy().
  - properties are only accessible through getters and setters.
  - these can be marked as immutable/readonly facilitating multi-threaded use.
  - todo: interesting, *i can do the same for my shaders*.
- various object types:
  - blobs: low level wrappers around binary data
  - faces: represent typefaces from a font file without any specific parameters set
  - fonts: instances of a face with all of their parameters specified
  - buffers: hold unicode codepoints before shaping and the shaped glyph output after shaping
  - shape plans: store the settings that harfbuzz will use when shaping a particular text segment

### blob
- a blob stores raw pieces of data like the raw font data
- makes it easy to pass raw data around
- functions
  - hb_blob_create

### buffers
- a buffer stores unicode strings
- from hb's perspective, it's just a stream of unicode codepoints
- functions for the buffer object
  - hb_buffer_create
  - hb_buffer_destroy
  - hb_buffer_reference
  - hb_buffer_reset
  - hb_buffer_clear_contents
- functions for unicode
  - hb_buffer_add_utf8
  - hb_buffer_add_utf16
  - hb_buffer_add_utf32
  - hb_buffer_add_codepoints
- these add functions append to the buffer
-  

