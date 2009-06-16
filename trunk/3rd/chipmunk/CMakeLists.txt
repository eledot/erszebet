add_definitions(-W -Wall -std=c99)
file(GLOB SRC_FILES *.c constraints/*.c)
add_library(chipmunk STATIC ${SRC_FILES})
