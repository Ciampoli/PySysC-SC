FILE(GLOB LibHeaders *.h)
set(LIB_HEADERS ${LibHeaders} )
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
