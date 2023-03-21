cmake_minimum_required (VERSION 2.8)

#set (CMAKE_CXX_STANDARD 11)
set(PROJECT_NAME_STR SceCellsGPU)
 
#set(CMAKE_CXX_COMPILER /opt/linux/centos/7.x/x86_64/easybuild/pkgs/GCCcore/4.9.3/bin/gcc CACHE PATH "" FORCE)
#set(CMAKE_CXX_COMPILER "/opt/linux/centos/7.x/x86_64/easybuild/pkgs/GCCcore/4.9.3/bin/gcc")
#set(CUDA_TOOLKIT_ROOT_DIR /opt/linux/centos/7.x/x86_64/pkgs/cuda/9.1)
#MESSAGE("CUDA_TOOLKIT_ROOT_DIR: " ${CUDA_TOOLKIT_ROOT_DIR})
	
project(${PROJECT_NAME_STR} LANGUAGES CXX CUDA)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.9.0")
    message(FATAL_ERROR "Insufficient gcc version")
  endif()
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "your.required.msvc.version")
    message(FATAL_ERROR "Insufficient msvc version")
  endif()
endif()


find_package(CUDA REQUIRED)

find_path( THRUST_INCLUDE_DIR
  HINTS
    ${CUDA_INCLUDE_DIRS}
	NAMES thrust/version.h
	DOC "Thrust headers"
)

if( THRUST_INCLUDE_DIR )

  list( REMOVE_DUPLICATES THRUST_INCLUDE_DIR )
endif( THRUST_INCLUDE_DIR )
MESSAGE("thrust include dir : " ${THRUST_INCLUDE_DIR}) 
MESSAGE("CUDA_INCLUDE_DIRS: " ${CUDA_INCLUDE_DIRS})


include_directories(${THRUST_INCLUDE_DIR}) #specifies path that compiler looks in while searching for header files


if(CMAKE_COMPILER_IS_GNUCXX)
        #add_definitions(-Wall -ansi -Wno-deprecated -fpermissive -frounding-math)
endif()

# After some performance testing, there is no significant performance difference
# between compute_20 and compute_30. 
# Therefore, for compatibility reason, compute 2.0 is used.
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -DTHRUST_DEBUG)
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -std=c++11)
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -gencode arch=compute_30,code=compute_30)
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -gencode arch=compute_50,code=compute_50)

# HPCC ADMIN ADDED
#set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -gencode arch=compute_37,code=sm_37)
set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS}; -gencode arch=compute_60,code=sm_60)

MESSAGE("NVCC flags are : " ${CUDA_NVCC_FLAGS})

# HPCC ADMIN ADDED
set(CMAKE_CUDA_ARCHITECTURES 37 60)

SET(CMAKE_BUILD_TYPE Debug)
SET(EXECUTABLE_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EXECUTABLE_PATH})
SET(PROJECT_BINARY_DIR ${PROJECT_SOURCE_DIR}/build)
SET(BUILD_DOCUMENTATION ON)

#cuda files and corresponding .h files are placed together in this folder
#set(COMMON_CUDA_INCLUDES ${PROJECT_SOURCE_DIR}/srcGPU)
#cpp files and corresponding .h files are placed together in this folder
#set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/srcCPU ${PROJECT_SOURCE_DIR}/srcGPU)
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/src/common)
# source code dir for GPU code
set(GPU_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/srcGPU)
# source code dir for CPU code
set(CPU_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src/srcCPU)
# source code dir for CPU Utils code
#set(CPU_SOURCE_UTIL_DIR ${PROJECT_SOURCE_DIR}/srcCPU/Utils)
#folder for external dependencies
set(EXT_PROJECTS_DIR ${PROJECT_SOURCE_DIR}/src/ext)
#set(PROJECT_EXE_DIR ${PROJECT_SOURCE_DIR}/exeFiles)
#location of executables
SET(CMAKE_BUILD_DIR ${CMAKE_SOURCE_DIR}/bin)
#location of unit test files
SET(CMAKE_UNITTEST_DIR ${CMAKE_SOURCE_DIR}/tst/unitTest)
#location of perfermance test files
SET(CMAKE_PERFTEST_DIR ${CMAKE_SOURCE_DIR}/tst/perfTest)
#SET(CMAKE_MODELTEST_DIR ${CMAKE_SOURCE_DIR}/modelTest)
SET(GOOGLETEST_DIR ${CMAKE_SOURCE_DIR}/tst/googletest)

# make VERBOSE =1

find_path(STDIO_INCLUDE_PATH stdio.h)
# later version c++ seems to be incompatible with the CUDA version we are using.
# Therefore, I have changed the system C++ version to make everything compatible.
# known compatible g++ version: 4.6.4

SET(STL_INCLUDE_PATH "${STDIO_INCLUDE_PATH}/c++")
SET(CMAKE_ECLIPSE_GENERATE_SOURCE_PROJECT TRUE)
include_directories(${STL_INCLUDE_PATH} ${STDIO_INCLUDE_PATH})

if(CUDA_FOUND)
        include_directories(${CUDA_INCLUDE_DIRS})
endif (CUDA_FOUND)


CUDA_INCLUDE_DIRECTORIES(
	${CMAKE_SOURCE_DIR}
	${CUDA_INSTALL_PREFIX}/include
	)
# for some reason, cusparse library is not included by default.
# therefore adding it here. 
#SET(CUDA_LIBRARIES "${CUDA_LIBRARIES};${CUDA_TOOLKIT_ROOT_DIR}/lib64/libcusparse.so")

#-- Add an Option to toggle the generation of the API documentation
#if(BUILD_DOCUMENTATION#)
#  message("Building documentation!")
#  FIND_PACKAGE(Doxygen)
#  if (NOT DOXYGEN_FOUND)
#    message(FATAL_ERROR 
#      "Doxygen is needed to build the documentation. Please install it correctly")
#  endif()
  #-- Configure the Template Doxyfile for our specific project
#  configure_file(Doxyfile.in 
#                 ${PROJECT_BINARY_DIR}/Doxyfile  @ONLY IMMEDIATE)
  #-- Add a custom target to run Doxygen when ever the project is built
#  add_custom_target (Docs ALL 
#  										COMMAND ${DOXYGEN_EXECUTABLE} ${PROJECT_BINARY_DIR}/Doxyfile
#  										SOURCES ${PROJECT_BINARY_DIR}/Doxyfile)
  # IF you do NOT want the documentation to be generated EVERY time you build the project
  # then leave out the 'ALL' keyword from the above command.
#endif()

MESSAGE("CMAKE_CXX_COMPLIER: " ${CMAKE_CXX_COMPLIER})
# This manual CXX FLAGS setting is required, because the some version of CMAKE would use an unsupported CMAKE_CXX_FLAGS and result in compliation failure.
# more specifically, -fstack-protector-strong option is not supported by g++ until g++-4.9, but it is enabled by default in CMake 2.8.12
# however g++-4.9 is not supported by nvcc 5.5
set(CMAKE_CXX_FLAGS "-g -std=c++11 -O0 -fstack-protector --param=ssp-buffer-size=4 -Wformat -Wformat-security  -frounding-math")
MESSAGE("manually resetting g++ flags to : " ${CMAKE_CXX_FLAGS})


	MESSAGE("CMAKE_CXX_COMPILER: " ${CMAKE_CXX_COMPILER})
#add_subdirectory(${EXT_PROJECTS_DIR}/gtest)
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/src)
ADD_SUBDIRECTORY(${CMAKE_SOURCE_DIR}/tst)

ENABLE_TESTING()


