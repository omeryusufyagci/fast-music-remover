#ifndef FFMPEGCONTROLLER_H
#define FFMPEGCONTROLLER_H

#include "FFmpegSettingsManager.h"

namespace MediaProcessor {

class FFmpegController {
   public:
    FFmpegController(const FFmpegSettingsManager& ffmpegSettings);

   private:
    FFmpegSettingsManager& m_ffmpegSettings;
};

}  // namespace MediaProcessor

#endif /* FFMPEGCONTROLLER_H */
