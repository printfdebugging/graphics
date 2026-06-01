
<!-- mtoc-start -->

* [C](#c)
  * [String functions](#string-functions)
    * [Formatting functions](#formatting-functions)
  * [Getting help from the compiler](#getting-help-from-the-compiler)
    * [Compiler flags](#compiler-flags)
    * [Address Sanitizers](#address-sanitizers)
  * [abstractions](#abstractions)
    * [error return codes](#error-return-codes)

<!-- mtoc-end -->
# C

> [!NOTE]
> Many (most) of the C functions have return values, some for success status,
> while others return the progress, like the number of bytes written/read.
> It's easy to not notice the return values but they are quite important, so
> look closely and handle them properly.

## String functions
### Formatting functions
- asprintf, vasprintf
- snprintf, vsnprintf
- https://en.wikipedia.org/wiki/C_string_handling#Functions
- TODO complete this

## Getting help from the compiler
### Compiler flags
- Read more here: [What are the useful GCC flags for C? - Stack Overflow](https://stackoverflow.com/questions/3375697/what-are-the-useful-gcc-flags-for-c)
- These help catch most subtle issues you can have
- `-Werror` treats warnings as errors, I don't like that, makes prototyping harder
- `-Wfloat-equal` had some issues with cglm, commented that out for now
```cmake
target_compile_options(${APPLICATION} PRIVATE
        -Wall
        -Wextra
        -Wundef
        -Wshadow
        -Wpointer-arith
        -Wcast-align
        -Wstrict-prototypes
        -Wwrite-strings
        -Wcast-qual
        -Wswitch-default
        -Wswitch-enum
        -Wconversion
        -Wunreachable-code
        -Wpedantic
        # -Wfloat-equal
        # -Werror
)
```

### Address Sanitizers
- These detect memory errors
- Just add `-fsanitize=undefined -fsanitize=address` to the block above
  and to `target_link_options`, see the root `CMakeLists.txt` in this
  project.
```bash
/home/printfdebugging/repos/learnopengl/source/utils.c:97:17: runtime error: null pointer passed as argument 2, which is declared to never be null
AddressSanitizer:DEADLYSIGNAL
=================================================================
==255355==ERROR: AddressSanitizer: SEGV on unknown address 0x7f5929925d20 (pc 0x7f5929925d20 bp 0x7ffd7517eae0 sp 0x7ffd7517eab8 T0)
==255355==The signal is caused by a READ memory access.
AddressSanitizer:DEADLYSIGNAL
AddressSanitizer: nested bug in the same thread, aborting.
make: *** [Makefile:2: run] Error 1
```
## abstractions
### error return codes
- cgltf's approach to error codes (as enums) is quite nice
