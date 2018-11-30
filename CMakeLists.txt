project(TransactionExample CXX)
cmake_minimum_required(VERSION 3.9)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(Conan)
setup_conan()

conan_basic_setup(TARGETS)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(components SHARED
	components/tx_example_mods.cpp 
	components/initiator.cpp 
	components/target.cpp 
	components/logging.cpp)
target_link_libraries(components PUBLIC CONAN_PKG::SystemC)
target_link_libraries(components PUBLIC CONAN_PKG::SystemCVerification)

add_executable(TransactionExample
	components/tx_example.cpp
	components/txtop.cpp)
target_link_libraries(TransactionExample components)

add_executable(router_example 
	components/router_example.cpp)
target_link_libraries(router_example components)

# CTest is a testing tool that can be used to test your project.
# enable_testing()
# add_test(NAME example
#          WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
#          COMMAND example)
