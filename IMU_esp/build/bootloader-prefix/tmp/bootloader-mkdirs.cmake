# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

# If CMAKE_DISABLE_SOURCE_CHANGES is set to true and the source directory is an
# existing directory in our source tree, calling file(MAKE_DIRECTORY) on it
# would cause a fatal error, even though it would be a no-op.
if(NOT EXISTS "/Users/sonalipalit/esp/esp-idf/components/bootloader/subproject")
  file(MAKE_DIRECTORY "/Users/sonalipalit/esp/esp-idf/components/bootloader/subproject")
endif()
file(MAKE_DIRECTORY
  "/Users/sonalipalit/Documents/ACADEMIC/Semester7/CS528/Silent-Speech-Recognition-with-Jaw-Motion-Sensors/IMU_esp/build/bootloader"
  "/Users/sonalipalit/Documents/ACADEMIC/Semester7/CS528/Silent-Speech-Recognition-with-Jaw-Motion-Sensors/IMU_esp/build/bootloader-prefix"
  "/Users/sonalipalit/Documents/ACADEMIC/Semester7/CS528/Silent-Speech-Recognition-with-Jaw-Motion-Sensors/IMU_esp/build/bootloader-prefix/tmp"
  "/Users/sonalipalit/Documents/ACADEMIC/Semester7/CS528/Silent-Speech-Recognition-with-Jaw-Motion-Sensors/IMU_esp/build/bootloader-prefix/src/bootloader-stamp"
  "/Users/sonalipalit/Documents/ACADEMIC/Semester7/CS528/Silent-Speech-Recognition-with-Jaw-Motion-Sensors/IMU_esp/build/bootloader-prefix/src"
  "/Users/sonalipalit/Documents/ACADEMIC/Semester7/CS528/Silent-Speech-Recognition-with-Jaw-Motion-Sensors/IMU_esp/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/Users/sonalipalit/Documents/ACADEMIC/Semester7/CS528/Silent-Speech-Recognition-with-Jaw-Motion-Sensors/IMU_esp/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/Users/sonalipalit/Documents/ACADEMIC/Semester7/CS528/Silent-Speech-Recognition-with-Jaw-Motion-Sensors/IMU_esp/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
