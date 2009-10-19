option(WITH_LUAJIT "Use LuaJIT" OFF)

add_subdirectory(chipmunk)

if(WITH_LUAJIT)
  add_subdirectory(luajit)
  message(">>> LuaJIT enabled")
else(WITH_LUAJIT)
  add_subdirectory(lua)
endif(WITH_LUAJIT)

if(APPLE)
  add_subdirectory(sdl)
endif(APPLE)
