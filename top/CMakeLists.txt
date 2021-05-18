cmake_minimum_required(VERSION 3.3)
set(APP_NAME top)
add_executable(${APP_NAME} sc_main.cpp)
target_link_libraries (${APP_NAME} LINK_PUBLIC vp_components)
