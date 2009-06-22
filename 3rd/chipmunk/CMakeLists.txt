add_definitions(-W -Wall -Wno-unused-parameter -Wno-unknown-pragmas -std=c99)
file(GLOB SRC_FILES *.c constraints/*.c)
add_library(chipmunk STATIC ${SRC_FILES})
