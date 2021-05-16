#include <stdint.h>
#include <VGA/VGA.h>

typedef struct {
    const char *filename;
    uint32_t line, column;
} source_location;

typedef struct {
    source_location location;
    source_location attr_loc;
    int arg_index;
} nonnull_data;

typedef struct {
    uint16_t type_kind, type_info;
    char* type_name;
} type_descriptor;

typedef struct {
    source_location location;
    type_descriptor *lhs_type, *rhs_type;
} shift_out_of_bounds_data;

typedef struct {
    source_location location;
    type_descriptor *type;
    unsigned char log_alignment;
    unsigned char type_check_kind;
} type_mismatch_data_v1;

static void ubsan_common(source_location location) {
    log("[UBSAN] at %s, line %d, column %d\n", location.filename, location.line, location.column);
    while (1);
}

void __ubsan_handle_nonnull_arg(nonnull_data *data) {
    log("[UBSAN] Nonnull attribute violation\n");
    ubsan_common(data->location);
}

void __ubsan_handle_shift_out_of_bounds(shift_out_of_bounds_data *data, unsigned long lhs, unsigned long rhs) {
    log("[UBSAN] Shift out of bounds (%lu `shift` %lu)", lhs, rhs);
    ubsan_common(data->location);
}


void __ubsan_handle_type_mismatch_v1(type_mismatch_data_v1 *data, unsigned long index) {
    log("[UBSAN] Type mismatch (ix: %lu, type: %s, align: %lu)\n", index, data->type->type_name, 1UL << data->log_alignment);
    ubsan_common(data->location);
}


#define HANDLER2(fun) void fun(void *data, unsigned long index) {log(#fun ": %p, %lu\n", data, index);}
#define HANDLER3(fun) void fun(void *data, unsigned long lhs, unsigned long rhs) {log(#fun ": %p, %lu, %lu\n", data, lhs, rhs);}

HANDLER2(__ubsan_handle_out_of_bounds)

HANDLER2(__ubsan_handle_load_invalid_value)

HANDLER2(__ubsan_handle_negate_overflow)

HANDLER3(__ubsan_handle_pointer_overflow)

HANDLER3(__ubsan_handle_divrem_overflow)

HANDLER3(__ubsan_handle_add_overflow)

HANDLER3(__ubsan_handle_mul_overflow)

HANDLER3(__ubsan_handle_sub_overflow)

HANDLER3(__ubsan_handle_vla_bound_not_positive)
