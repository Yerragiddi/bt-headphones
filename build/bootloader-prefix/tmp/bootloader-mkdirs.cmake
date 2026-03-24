# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/manohar/esp-idf/components/bootloader/subproject"
  "/home/manohar/bt-headphones/build/bootloader"
  "/home/manohar/bt-headphones/build/bootloader-prefix"
  "/home/manohar/bt-headphones/build/bootloader-prefix/tmp"
  "/home/manohar/bt-headphones/build/bootloader-prefix/src/bootloader-stamp"
  "/home/manohar/bt-headphones/build/bootloader-prefix/src"
  "/home/manohar/bt-headphones/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/manohar/bt-headphones/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/manohar/bt-headphones/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
