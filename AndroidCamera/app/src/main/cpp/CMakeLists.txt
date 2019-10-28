# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

# 不继寻找上一级目录
message(WARNING ${CMAKE_SOURCE_DIR})
get_filename_component(PARENT_DIR ${CMAKE_SOURCE_DIR} PATH)
get_filename_component(PARENT_DIR ${PARENT_DIR} PATH)
get_filename_component(PARENT_DIR ${PARENT_DIR} PATH)
message(WARNING ${PARENT_DIR})


#设置库目录
set(LIBRARY_DIR ${PARENT_DIR}/libs)

# ${ANDROID_ABI}的值在模块的build.gradle文件的android.defaultConfig.externalNativeBuild.ndk.abiFilters中设置

#引入已经编译好的外部库，名称赋值为ffmpeg-single
add_library(ffmpeg-single
        SHARED
        IMPORTED)

#指定ffmpeg-single库的路径
set_target_properties(ffmpeg-single
        PROPERTIES IMPORTED_LOCATION
        ${LIBRARY_DIR}/${ANDROID_ABI}/libffmpeg.so
        )

message(WARNING ${LIBRARY_DIR}/${ANDROID_ABI}/libffmpeg.so)

#头文件查找目录
include_directories(${LIBRARY_DIR}/${ANDROID_ABI}/include)

message(WARNING ${LIBRARY_DIR})
message(WARNING ${LIBRARY_DIR}/${ANDROID_ABI}/include)
#message(FATAL_ERROR "i am a debug message^_^")

add_library( # Sets the name of the library.
             bridge

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             bridge.c )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
              log-lib

              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
                       bridge

                       # Links the target library to the log library
                       # included in the NDK.
                       ffmpeg-single
                       ${log-lib} )