#ifndef HARDWAREUTILS_H
#define HARDWAREUTILS_H

namespace MediaProcessor::HardwareUtils {

/**
 * @brief Default number of threads used when hardware thread count is unavailable.
 */
constexpr unsigned int DEFAULT_NUM_THREADS = 6;

/**
 * @brief Retrieves the number of hardware threads available on the system.
 *
 * @return The number of hardware threads.
 */
unsigned int getHardwareThreadCount();

}  // namespace MediaProcessor::HardwareUtils

#endif  // HARDWAREUTILS_H
