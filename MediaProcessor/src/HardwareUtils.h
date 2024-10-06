#ifndef HARDWAREUTILS_H
#define HARDWAREUTILS_H

namespace MediaProcessor::HardwareUtils {

constexpr unsigned int DEFAULT_NUM_THREADS = 6;
unsigned int getHardwareThreadCount();

}  // namespace MediaProcessor

#endif  // HARDWAREUTILS_H
