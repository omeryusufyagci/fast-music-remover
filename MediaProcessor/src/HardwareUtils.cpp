#include "HardwareUtils.h"

#include <thread>

namespace MediaProcessor::HardwareUtils {

unsigned int getHardwareThreadCount() {
    /*
     * If the value is not well-defined or not computable, hardware_concurrency() may return 0.
     * In that case, we'll fall back to DEFAULT_NUM_CHUNKS.
     * We subtract 2 from the available hardware threads as a safety margin to avoid overloading the
     * system.
     */
    unsigned int hardwareThreadCount = std::thread::hardware_concurrency();
    if (hardwareThreadCount > 0) {
        return (hardwareThreadCount > 2) ? (hardwareThreadCount - 2) : 1;
    }
    return DEFAULT_NUM_THREADS;
}

}  // namespace MediaProcessor::HardwareUtils