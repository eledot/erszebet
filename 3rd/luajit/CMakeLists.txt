file(GLOB SRC_FILES *.c)
add_library(lua STATIC ${SRC_FILES})
