#ifndef DEEPFILTERNET_FFI_H
#define DEEPFILTERNET_FFI_H

#include <cstddef>
#include <cstdint>

/*
 * Compile the libdf with `cargo build --release --features capi`
 * Otherwise doesn't include the C API
 */

struct DynArray {
    uint32_t* array;
    uint32_t length;
};

// Struct forward declaration
struct DFState;

extern "C" {

DFState* df_create(const char* path, float atten_lim, const char* log_level);

size_t df_get_frame_length(DFState* st);

char* df_next_log_msg(DFState* st);

void df_free_log_msg(char* ptr);
void df_set_atten_lim(DFState* st, float lim_db);
void df_set_post_filter_beta(DFState* st, float beta);
void df_free(DFState* model);

float df_process_frame(DFState* st, float* input, float* output);
float df_process_frame_raw(DFState* st, float* input, float** out_gains_p, float** out_coefs_p);

DynArray df_coef_size(const DFState* st);
DynArray df_gain_size(const DFState* st);
}

#endif  // DEEPFILTERNET_FFI_H
