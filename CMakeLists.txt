cmake_minimum_required(VERSION 3.17)
project(AVencoder)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")

include_directories("/home/ubu-dev/ffmpeg_build/include")
link_directories("/home/ubu-dev/ffmpeg_build/lib")

include_directories("/home/ubu-dev/Developer/SVT-HEVC/Source/API")
link_directories("/home/ubu-dev/Developer/SVT-HEVC/Bin/Release")

add_executable(AVencoder main.cpp)
target_link_libraries(AVencoder avcodec avformat avutil avdevice SvtHevcEnc pthread m z swresample lzma)