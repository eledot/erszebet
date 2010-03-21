file(GLOB SRC_FILES *.c constraints/*.c)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/contraints)
add_definitions(-DTARGET_OS_IPHONE=0)
add_library(chipmunk STATIC ${SRC_FILES})
