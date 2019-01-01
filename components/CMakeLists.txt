set(LIB_SOURCES
	initiator.cpp
	logging.cpp
	target.cpp
)

# Define two variables in order not to repeat ourselves.
set(LIBRARY_NAME components)
# Define the library
add_library(${LIBRARY_NAME} SHARED ${LIB_SOURCES})
target_link_libraries (${LIBRARY_NAME} LINK_PUBLIC sc-components)
target_include_directories (components PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
