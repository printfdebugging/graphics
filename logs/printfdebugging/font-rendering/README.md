
<!-- mtoc-start -->

* [font rendering](#font-rendering)
  * [harfbuzz](#harfbuzz)
  * [getting started](#getting-started)
  * [terminology](#terminology)
  * [hb-gpu](#hb-gpu)

<!-- mtoc-end -->
# font rendering
## harfbuzz
- these are the resources which seem quite useful at the first look
- [Getting started with HarfBuzz: HarfBuzz Manual](https://harfbuzz.github.io/getting-started.html)
- [Terminology: HarfBuzz Manual](https://harfbuzz.github.io/terminology.html)
- [A simple shaping example: HarfBuzz Manual](https://harfbuzz.github.io/a-simple-shaping-example.html)
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
