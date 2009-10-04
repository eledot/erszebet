set(CMAKE_C_FLAGS "-W -Wall -Wextra -std=c99")

add_subdirectory(chipmunk)
add_subdirectory(lua)
if(APPLE)
  add_subdirectory(sdl)
endif(APPLE)
