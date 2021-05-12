#include <stdint.h>
#include <VGA.h>

typedef struct {
    const char *filename;
    uint32_t line, column;
} source_location;

typedef struct {
    source_location loc;
    source_location attr_loc;
    int arg_index;
} nonnull_data;

typedef struct {
    uint16_t type_kind, type_info;
    char type_name[1];
} type_descriptor;

typedef struct {
    source_location loc;
    type_descriptor *lhs_type, *rhs_type;
} shift_out_of_bounds_data;

void __ubsan_handle_nonnull_arg(nonnull_data *data) {
    log("[UBSAN] nonnull attribute violation at %s, line %d\n", data->loc.filename, data->loc.line);
    while (1);
}

void __ubsan_handle_shift_out_of_bounds(shift_out_of_bounds_data *data, unsigned long lhs, unsigned long rhs) {
    log("[UBSAN] shift out of bounds (%lu `shift` %lu) at %s, line %d\n", lhs, rhs, data->loc.filename, data->loc.line);
    while (1);
}

#define HANDLER2(fun) void fun(void *data, unsigned long index) {log(#fun ": %p, %lu\n", data, index);}
#define HANDLER3(fun) void fun(void *data, unsigned long lhs, unsigned long rhs) {log(#fun ": %p, %lu, %lu\n", data, lhs, rhs);}


HANDLER2(__ubsan_handle_type_mismatch_v1)

HANDLER2(__ubsan_handle_out_of_bounds)

HANDLER2(__ubsan_handle_load_invalid_value)

HANDLER2(__ubsan_handle_negate_overflow)

HANDLER3(__ubsan_handle_pointer_overflow)

HANDLER3(__ubsan_handle_divrem_overflow)

HANDLER3(__ubsan_handle_add_overflow)

HANDLER3(__ubsan_handle_mul_overflow)
