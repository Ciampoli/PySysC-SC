cmake_minimum_required(VERSION 3.3)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_CURRENT_SOURCE_DIR}/scc/cmake)

project(pysysc-sc)

set(ENABLE_SHARED TRUE CACHE BOOL "Build shared libraries")

include(GNUInstallDirs)
include(BuildType)
include(clang-format)

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

include(ConanInline)
conan_check()
set(CONAN_BOOST_OPTIONS
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
set(CONAN_PACKAGES boost/1.75.0)
set(CONAN_SETTINGS ${CONAN_BOOST_OPTIONS})
if(NOT DEFINED ENV{SYSTEMC_HOME})
    set(CONAN_PACKAGES ${CONAN_PACKAGES}
        systemc/2.3.3
        systemc-cci/1.0.0
        )
    set(CONAN_SETTINGS ${CONAN_SETTINGS}  
        #systemc:phase_cb=False
        systemc-cci:shared=False
        )
endif()
conan_configure(REQUIRES ${CONAN_PACKAGES}
                GENERATORS cmake_find_package
                OPTIONS ${CONAN_SETTINGS}
                )
conan_install()
conan_setup(TARGETS)

# This line finds the boost lib and headers. 
set(Boost_NO_BOOST_CMAKE ON) #  Don't do a find_package in config mode before searching for a regular boost install.
find_package(Boost COMPONENTS program_options system thread REQUIRED)

# set-up SystemC and SCV
include(SystemCPackage)
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

add_subdirectory(scc)
add_subdirectory(vp_components)
add_subdirectory(top)

# CTest is a testing tool that can be used to test your project.
# enable_testing()
# add_test(NAME example
#          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
#          COMMAND example)
