add_definitions(-W -Wall -std=c99)
file(GLOB SRC_FILES *.c)
add_library(lua STATIC ${SRC_FILES})
