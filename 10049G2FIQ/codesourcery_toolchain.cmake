INCLUDE(CMakeForceCompiler)

# this one is important
SET(CMAKE_SYSTEM_NAME Generic)
#this one not so much
SET(CMAKE_SYSTEM_VERSION 1)

SET(CMAKE_VERBOSE_MAKEFILE ON)
 
# specify the cross compiler
CMAKE_FORCE_C_COMPILER(/opt/CC/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux/bin/arm-none-linux-gnueabi-gcc GNU)
CMAKE_FORCE_CXX_COMPILER(/opt/CC/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux/bin/arm-none-linux-gnueabi-g++ GNU)
SET(CMAKE_STRIP /opt/CC/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux/bin/arm-linux-gnueabi-strip)
 
SET(COMMON_FLAGS "-march=armv5te -mtune=arm926ej-s -Wall -O2" CACHE STRING "" FORCE)
SET(CMAKE_C_FLAGS "${COMMON_FLAGS} " CACHE STRING "" FORCE)
SET(CMAKE_CXX_FLAGS "${COMMON_FLAGS} -std=gnu++0x" CACHE STRING "" FORCE)
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Map,10049.map" CACHE STRING "" FORCE)

# where is the target environment 
SET(CMAKE_FIND_ROOT_PATH  /opt/LIB/CFA921-usr /opt/CC/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux/arm-none-linux-gnueabi/libc/lib)

# search for programs in the build host directories
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
# for libraries and headers in the target directories
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

SET(CMAKE_FIND_LIBRARY_PREFIXES lib)
SET(CMAKE_FIND_LIBRARY_SUFFIXES .so)

