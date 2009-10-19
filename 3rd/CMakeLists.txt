add_subdirectory(chipmunk)

if(WITH_LUAJIT)
  add_subdirectory(luajit)
else(WITH_LUAJIT)
  add_subdirectory(lua)
endif(WITH_LUAJIT)

if(APPLE)
  add_subdirectory(sdl)
endif(APPLE)
