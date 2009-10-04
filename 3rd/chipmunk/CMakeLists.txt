file(GLOB SRC_FILES *.c constraints/*.c)
add_library(chipmunk STATIC ${SRC_FILES})
