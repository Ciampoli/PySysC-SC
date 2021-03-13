cmake_minimum_required(VERSION 3.3)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_CURRENT_SOURCE_DIR}/sc-components/cmake)

project(pysysc-sc)

set(ENABLE_SCV TRUE CACHE BOOL "Enable use of SCV")
set(ENABLE_SHARED TRUE CACHE BOOL "Build shared libraries")


include(GNUInstallDirs)
include(Conan)
include(BuildType)

#enable_testing() 

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
if("${CMAKE_BUILD_TYPE}" STREQUAL "") 
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
elseif(NOT(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo"))
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" OR "${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    set(warnings "-Wall -Wextra -Werror")
    set(CMAKE_CXX_FLAG_RELEASE "-O3 -DNDEBUG")
    set(CMAKE_C_FLAG_RELEASE "-O3 -DNDEBUG")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(warnings "/W4 /WX /EHsc")
endif()

setup_conan()

# This line finds the boost lib and headers. 
set(Boost_NO_BOOST_CMAKE ON) #  Don't do a find_package in config mode before searching for a regular boost install.
find_package(Boost COMPONENTS program_options system thread REQUIRED)

# set-up SystemC and SCV
find_package(OSCISystemC)
if(SystemC_FOUND)
    include_directories(${SystemC_INCLUDE_DIRS})
    link_directories(${SystemC_LIBRARY_DIRS})
else()
    message( FATAL_ERROR "SystemC library not found." )
endif()

if(CCI_FOUND)
    include_directories(${CCI_INCLUDE_DIRS})
    link_directories(${CCI_LIBRARY_DIRS})
#else()
    #message( FATAL_ERROR "SystemC CCI library not found." )
endif()

include(sc-components/cmake/clang-format.cmake)

add_subdirectory(sc-components)
add_subdirectory(components)
add_subdirectory(top)

# CTest is a testing tool that can be used to test your project.
# enable_testing()
# add_test(NAME example
#          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
#          COMMAND example)
