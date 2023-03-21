cmake_minimum_required(VERSION 3.3)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_CURRENT_SOURCE_DIR}/scc/cmake)

project(pysysc-sc)

set(ENABLE_SHARED TRUE CACHE BOOL "Build shared libraries")

include(GNUInstallDirs)
include(BuildType)
include(clang-format)

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

include(ConanInline)
conan_check()
set(BOOST_OPTIONS
boost:fPIC=True
boost:shared=True
boost:header_only=False 
boost:without_context=True
boost:without_contract=True
boost:without_coroutine=True
boost:without_fiber=True
boost:without_graph=True
boost:without_graph_parallel=True
boost:without_iostreams=True
boost:without_json=True
boost:without_locale=True
boost:without_log=True
boost:without_math=True
boost:without_mpi=True
boost:without_nowide=True
boost:without_python=True
boost:without_random=True
boost:without_regex=True
boost:without_serialization=True
boost:without_stacktrace=True
boost:without_test=True
boost:without_timer=True
boost:without_type_erasure=True
boost:without_wave=True
)
set(CONAN_PACKAGE_LIST fmt/8.0.1 spdlog/1.9.2 zlib/1.2.11 boost/1.75.0 yaml-cpp/0.7.0 jsoncpp/1.9.5 lz4/1.9.3)
set(CONAN_PACKAGE_OPTIONS fmt:header_only=True spdlog:header_only=True ${BOOST_OPTIONS})
if(NOT USE_CWR_SYSTEMC AND NOT USE_NCSC_SYSTEMC AND NOT DEFINED ENV{SYSTEMC_HOME})
    set(CONAN_PACKAGE_LIST ${CONAN_PACKAGE_LIST} systemc/2.3.3 systemc-cci/1.0.0)
    set(CONAN_PACKAGE_OPTIONS ${CONAN_PACKAGE_OPTIONS} systemc-cci:shared=False)
endif()
conan_configure(REQUIRES ${CONAN_PACKAGE_LIST}
                GENERATORS cmake_find_package
                OPTIONS ${CONAN_PACKAGE_OPTIONS}
                )
conan_install()
set(CONAN_CMAKE_SILENT_OUTPUT ON)
# This line finds the boost lib and headers. 
set(Boost_NO_BOOST_CMAKE ON) #  Don't do a find_package in config mode before searching for a regular boost install.
find_package(Boost COMPONENTS program_options system thread REQUIRED)

include(SystemCPackage)
find_package(spdlog REQUIRED)
find_package(fmt REQUIRED)
find_package(yaml-cpp REQUIRED)
find_package(lz4 REQUIRED)

add_subdirectory(scc)
add_subdirectory(vp_components)
add_subdirectory(top)

# CTest is a testing tool that can be used to test your project.
# enable_testing()
# add_test(NAME example
#          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
#          COMMAND example)
