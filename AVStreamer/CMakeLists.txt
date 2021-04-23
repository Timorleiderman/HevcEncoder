cmake_minimum_required(VERSION 3.19)
project(AVStreamer)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-D__STDC_CONSTANT_MACROS")

include_directories("/home/ubu-admin/ffmpeg_build/include")
link_directories("/home/ubu-admin/ffmpeg_build/lib")

include_directories("/home/ubu-admin/Developer/SVT-HEVC/Source/API")
link_directories("/home/ubu-admin/Developer/SVT-HEVC/Bin/Release")

add_executable(AVStreamer main.c)
target_link_libraries(AVStreamer avformat avcodec swscale avutil avfilter swresample avdevice postproc ao z lzma bz2 SvtHevcEnc pthread m)


