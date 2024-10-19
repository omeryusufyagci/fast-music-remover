#ifndef DEEPFILTERNET_FFI_H
#define DEEPFILTERNET_FFI_H

#include <cstddef>
#include <cstdint>

/*
 * @brief Ensure the libdf is compiled with `cargo build --release --features capi`
 * to include the C API.
 */

/**
 * @brief Represents a dynamic array with a length and pointer to the array data.
 */
struct DynArray {
    uint32_t* array;
    uint32_t length;
};

// Forward declaration of DFState struct
struct DFState;

extern "C" {

/**
 * @brief Generates a DeepFilterNet state instance.
 *
 * @param path Path to the model.
 * @param atten_lim Attenuation limit in dB.
 * @param log_level Logging level (can be NULL)
 * @return Pointer to the DFState instance.
 */
DFState* df_create(const char* path, float atten_lim, const char* log_level);

/**
 * @brief Gets the frame length required by the model.
 *
 * @param st Pointer to the DFState instance.
 * @return Frame length.
 */
size_t df_get_frame_length(DFState* st);

/**
 * @brief Retrieves the next log message, if available.
 *
 * @param st Pointer to the DFState instance.
 * @return Pointer to a string containing the log message.
 */
char* df_next_log_msg(DFState* st);

/**
 * @brief Frees a previously allocated log message.
 *
 * @param ptr Pointer to the log message to free.
 */
void df_free_log_msg(char* ptr);

/**
 * @brief Sets the attenuation limit for the DeepFilterNet model.
 *
 * @param st Pointer to the DFState instance.
 * @param lim_db New attenuation limit in dB.
 */
void df_set_atten_lim(DFState* st, float lim_db);

/**
 * @brief Sets the post-filter beta parameter.
 *
 * @param st Pointer to the DFState instance.
 * @param beta The new beta value.
 */
void df_set_post_filter_beta(DFState* st, float beta);

/**
 * @brief Frees the DeepFilterNet model instance.
 *
 * @param model Pointer to the DFState instance to free.
 */
void df_free(DFState* model);

/**
 * @brief Processes (filters) a frame of audio data.
 *
 * @param st Pointer to the DFState instance.
 * @param input Input audio frame data.
 * @param output Output audio frame data.
 * @return The attenuation factor applied.
 */
float df_process_frame(DFState* st, float* input, float* output);

/**
 * @brief Processes (filters) a frame of audio data and provides raw gains and coefficients.
 *
 * @param st Pointer to the DFState instance.
 * @param input Input audio frame data.
 * @param out_gains_p Pointer to output gains.
 * @param out_coefs_p Pointer to output coefficients.
 * @return The attenuation factor applied.
 */
float df_process_frame_raw(DFState* st, float* input, float** out_gains_p, float** out_coefs_p);

/**
 * @brief Retrieves the size of the coefficients.
 *
 * @param st Pointer to the DFState instance.
 * @return DynArray representing the coefficient size.
 */
DynArray df_coef_size(const DFState* st);

/**
 * @brief Retrieves the size of the gains.
 *
 * @param st Pointer to the DFState instance.
 * @return DynArray representing the gain size.
 */
DynArray df_gain_size(const DFState* st);
}

#endif  // DEEPFILTERNET_FFI_H
