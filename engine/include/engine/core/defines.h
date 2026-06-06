#ifndef CORE_DEFINES_H
#define CORE_DEFINES_H

#include <stdint.h>

typedef uint_least8_t u8;
typedef uint_least16_t u16;
typedef uint_least32_t u32;
typedef uint_least64_t u64;

typedef int_least8_t i8;
typedef int_least16_t i16;
typedef int_least32_t i32;
typedef int_least64_t i64;

typedef float f32;
typedef double f64;

typedef bool b8;
typedef int b32;

#if defined(__clang__) || defined(__GNUC__)
#define STATIC_ASSERT _Static_assert
#else
#define STATIC_ASSERT static_assert
#endif

STATIC_ASSERT(sizeof(u8) == 1, "Expected u8 to be 1 byte.");
STATIC_ASSERT(sizeof(u16) == 2, "Expected u16 to be 2 bytes.");
STATIC_ASSERT(sizeof(u32) == 4, "Expected u32 to be 4 bytes.");
STATIC_ASSERT(sizeof(u64) == 8, "Expected u64 to be 8 bytes.");

STATIC_ASSERT(sizeof(i8) == 1, "Expected i8 to be 1 byte.");
STATIC_ASSERT(sizeof(i16) == 2, "Expected i16 to be 2 bytes.");
STATIC_ASSERT(sizeof(i32) == 4, "Expected i32 to be 4 bytes.");
STATIC_ASSERT(sizeof(i64) == 8, "Expected i64 to be 8 bytes.");

STATIC_ASSERT(sizeof(f32) == 4, "Expected f32 to be 4 bytes.");
STATIC_ASSERT(sizeof(f64) == 8, "Expected f64 to be 8 bytes.");

#define U64_MAX 18446744073709551615UL
#define U32_MAX 4294967295U
#define U16_MAX 65535U
#define U8_MAX  255U
#define U64_MIN 0UL
#define U32_MIN 0U
#define U16_MIN 0U
#define U8_MIN  0U

#define I8_MAX  127
#define I16_MAX 32767
#define I32_MAX 2147483647
#define I64_MAX 9223372036854775807L
#define I8_MIN  (-I8_MAX - 1)
#define I16_MIN (-I16_MAX - 1)
#define I32_MIN (-I32_MAX - 1)
#define I64_MIN (-I64_MAX - 1)

enum status_code {
        /* success and failure */
        status_success = 0,
        status_failure = 1,

        /* caller / programmer errors (2-99) */
        status_invalid_argument = 2,
        status_invalid_state,
        status_unexpected_null_found,
        status_out_of_range,
        status_not_supported,
        status_not_implemented,
        status_already_initialized,
        status_not_initialized,
        status_buffer_too_small,

        /* system / runtime failures (100-199) */
        status_io_failure = 100,
        status_file_not_found,
        status_permission_denied,
        status_path_not_found,
        status_allocation_failed,
        status_timeout,
        status_resource_busy,
        status_resource_exhausted,
        status_system_failure,

        /* external library failures (200-299) */
        status_library_init_failure = 200,
        status_library_call_failure,
        status_library_version_mismatch,

        /* data / serialization failures (300-399) */
        status_parse_failure = 300,
        status_invalid_format,
        status_corrupted_data,
        status_checksum_failure,
        status_end_of_file,

        /* graphics / rendering failures (400-499) */
        status_shader_compile_failure = 400,
        status_shader_link_failure,
        status_asset_load_failure,
        status_texture_load_failure,

        /* networking failures (500-599) */
        status_connection_failed = 500,
        status_connection_lost,
        status_host_unreachable,
        status_protocol_error,

        /* generic catch-all */
        status_unknown_failure = 1000,
};

typedef enum status_code status_code;

#endif
