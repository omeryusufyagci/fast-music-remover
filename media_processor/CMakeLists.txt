cmake_minimum_required(VERSION 3.10)
project(VideoSpeechProcessing)

set(CMAKE_CXX_STANDARD 17)  # c++17 or above required

include_directories(include)

add_executable(VideoSpeechProcessing src/main.cpp src/SpeechIsolation.cpp src/VideoProcessing.cpp)

find_package(PkgConfig REQUIRED) # uses pkg-config to link ffmpeg libs
pkg_check_modules(FFMPEG REQUIRED libavcodec libavformat libavfilter)

target_include_directories(VideoSpeechProcessing PRIVATE ${FFMPEG_INCLUDE_DIRS})
target_link_libraries(VideoSpeechProcessing PRIVATE ${FFMPEG_LIBRARIES})
