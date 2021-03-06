#   Copyright (C) 2009 Serge "ftrvxmtrx" Ziryukin
#
#   This program is free software; you can redistribute it and/or
#   modify it under the terms of the GNU Lesser General Public
#   License as published by the Free Software Foundation; either
#   version 3 of the License, or (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#   Lesser General Public License for more details.
#
#   You should have received a copy of the GNU Lesser General
#   Public License along with this library; if not, write to the
#   Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
#   Boston, MA 02110-1301 USA

cmake_minimum_required(VERSION 2.6)
project(erszebet)

set(P ${PROJECT_NAME})

set(${P}_MAJOR_VERSION 0)
set(${P}_MINOR_VERSION 1)
set(${P}_PATCH_LEVEL   0)

option(WITH_LUAJIT "Use LuaJIT" OFF)

option(WITH_IMAGE_PNG   "PNG image format support" ON)
option(WITH_IMAGE_JPEG  "JPEG image format support" ON)
option(WITH_IMAGE_CG    "PNG/JPEG image format support (CoreGraphics)" ON)
option(WITH_IMAGE_PVRTC "PVRTC image format support" OFF)

option(WITH_SND      "enable sound" ON)
option(WITH_SND_WAV  "WAV audio format support" ON)
option(WITH_SND_OGG  "OGG VORBIS audio format support" ON)
option(WITH_SND_FLAC "FLAC audio format support" ON)

option(RELEASE               "release build" OFF)
option(DEBUG_SYS_PRINTF      "print debug messages" ON)
option(DEBUG_SYS_PRINTF_LOC  "print location in debug messages" ON)
option(DEBUG_SYS_PRINTF_TIME "print timestamp in debug messages" OFF)

option(DEBUG_SND "debug sound" ON)
option(DEBUG_GL  "debug opengl" ON)
option(DEBUG_MEM "debug memory manager" ON)

set(COMPILE_FLAGS "-W -Wall -Wextra -Wmissing-declarations -std=c99")
add_definitions(-DGAME_NAME=\"${P}\")

if(RELEASE)
  set(COMPILE_FLAGS "-O3 ${COMPILE_FLAGS}")
  message(">>> RELEASE enabled")
  add_definitions(-DRELEASE=1)
else(RELEASE)
  set(COMPILE_FLAGS "-O0 -ggdb ${COMPILE_FLAGS}")
  message(">>> DEBUG enabled")
  add_definitions(-DDEBUG=1)
endif(RELEASE)

add_subdirectory(3rd)
add_subdirectory(tools)

set(DIR_3RD ${CMAKE_CURRENT_SOURCE_DIR}/3rd)
include_directories(BEFORE "${DIR_3RD}/sglib")
set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wformat-security -Wundef")

if(WITH_LUAJIT)
  add_definitions(-I${DIR_3RD}/luajit -DENGINE_LUA_JIT)
  message(">>> LuaJIT enabled")
else(WITH_LUAJIT)
  add_definitions(-I${DIR_3RD}/lua)
endif(WITH_LUAJIT)

add_definitions(-I${DIR_3RD}/chipmunk -DTARGET_OS_IPHONE=0)
add_definitions(-I${CMAKE_CURRENT_SOURCE_DIR}/src)

if(NOT RELEASE)
  if(DEBUG_SYS_PRINTF)
    add_definitions(-DENGINE_SYS_PRINTF)
    if(DEBUG_SYS_PRINTF_LOC)
      add_definitions(-DENGINE_SYS_PRINTF_LOCATION)
    endif(DEBUG_SYS_PRINTF_LOC)
    if(DEBUG_SYS_PRINTF_TIME)
      add_definitions(-DENGINE_SYS_PRINTF_TIMESTAMP)
    endif(DEBUG_SYS_PRINTF_TIME)
  endif(DEBUG_SYS_PRINTF)
  if(DEBUG_SND)
    add_definitions(-DENGINE_SND_DEBUG)
  endif(DEBUG_SND)
  if(DEBUG_GL)
    add_definitions(-DENGINE_GL_DEBUG)
  endif(DEBUG_GL)
  if(DEBUG_MEM)
    add_definitions(-DENGINE_MEM_DEBUG)
  endif(DEBUG_MEM)
endif(NOT RELEASE)

file(GLOB SRC_FILES src/*.c src/*/*.c)
add_executable(${P} ${SRC_FILES})
add_dependencies(${P} chipmunk)
add_dependencies(${P} lua)
target_link_libraries(${P} chipmunk lua)
set_target_properties(${P} chipmunk lua PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS}")

if(NOT CMAKE_C_COMPILER MATCHES ".*/clang.*")
  set(COMPILE_FLAGS "${COMPILE_FLAGS} -Wswitch-default -Wswitch-enum")
endif(NOT CMAKE_C_COMPILER MATCHES ".*/clang.*")

if(APPLE)
  add_definitions(-DENGINE_OS_APPLE)
  add_dependencies(${P} sdl)
  target_link_libraries(${P} sdl)

  if(WITH_IMAGE_CG)
    find_library(APPSRVFR ApplicationServices)
    set(CGH ${APPSRVFR}/Frameworks/CoreGraphics.framework/Headers)
    if(EXISTS ${CGH}/CoreGraphics.h)
      include_directories(${CGH})
      add_definitions(-DENGINE_IMAGE_CG)
      message(">>> PNG/JPEG image support via CG enabled")
    endif(EXISTS ${CGH}/CoreGraphics.h)
  endif(WITH_IMAGE_CG)
else(APPLE)
  include(FindSDL_ttf)
  if(SDLTTF_FOUND)
    include_directories(${SDLTTF_INCLUDE_DIR})
    target_link_libraries(${P} ${SDLTTF_LIBRARY})
  else(SDL_FOUND)
    message(FATAL_ERROR "SDL_ttf not found")
  endif(SDLTTF_FOUND)
endif(APPLE)

include(FindOpenGL)
if(OPENGL_FOUND)
  if(APPLE)
    include_directories(${OPENGL_INCLUDE_DIR}/Headers)
  else(APPLE)
    include_directories(${OPENGL_INCLUDE_DIR}/GL)
  endif(APPLE)
  target_link_libraries(${P} ${OPENGL_gl_LIBRARY})
else(OPENGL_FOUND)
  message(FATAL_ERROR "OpenGL not found")
endif(OPENGL_FOUND)

include(FindSDL)
if(SDL_FOUND)
  include_directories(${SDL_INCLUDE_DIR})
  target_link_libraries(${P} ${SDL_LIBRARY})
  add_definitions(-DENGINE_VIDEO_SDL)
else(SDL_FOUND)
  message(FATAL_ERROR "SDL not found")
endif(SDL_FOUND)

if(WITH_IMAGE_PNG)
  include(FindPNG)
  if(PNG_FOUND)
    include_directories(${PNG_INCLUDE_DIR})
    target_link_libraries(${P} ${PNG_LIBRARIES})
    add_definitions(-DENGINE_IMAGE_PNG ${PNG_DEFINITIONS})
    message(">>> PNG support enabled")
  endif(PNG_FOUND)
endif(WITH_IMAGE_PNG)

if(WITH_IMAGE_JPEG)
  include(FindJPEG)
  if(JPEG_FOUND)
    include_directories(${JPEG_INCLUDE_DIR})
    target_link_libraries(${P} ${JPEG_LIBRARIES})
    add_definitions(-DENGINE_IMAGE_JPEG)
    message(">>> JPEG support enabled")
  endif(JPEG_FOUND)
endif(WITH_IMAGE_JPEG)

if(WITH_IMAGE_PVRTC)
    add_definitions(-DENGINE_IMAGE_PVRTC)
    message(">>> PVRTC support enabled")
endif(WITH_IMAGE_PVRTC)

if(WITH_SND)
  include(FindOpenAL)
  if(OPENAL_FOUND)
    include_directories(${OPENAL_INCLUDE_DIR})
    target_link_libraries(${P} ${OPENAL_LIBRARY})
    add_definitions(-DENGINE_SND)

    if(WITH_SND_WAV)
      add_definitions(-DENGINE_SND_WAV)
      message(">>> WAV support enabled")
    endif(WITH_SND_WAV)

    if(WITH_SND_OGG OR WITH_SND_FLAC)
      include(FindPkgConfig)
      if(PKG_CONFIG_FOUND)
        include (CheckLibraryExists)

        if(WITH_SND_OGG)
          pkg_check_modules(OGG vorbisfile)
          if(OGG_FOUND)
            find_library(VORBISFILE_LIB NAMES vorbisfile)
            if(EXISTS ${VORBISFILE_LIB})
              add_definitions(-DENGINE_SND_OGG ${OGG_CFLAGS})
              target_link_libraries(${P} ${VORBISFILE_LIB})
              foreach(OGGINC ${CMAKE_SYSTEM_PREFIX_PATH})
                if(EXISTS ${OGGINC}/include/vorbis/vorbisfile.h)
                  message("-- found vorbisfile.h in ${OGGINC}/include")
                  include_directories(${OGGINC}/include/vorbis)
                  break()
                endif(EXISTS ${OGGINC}/include/vorbis/vorbisfile.h)
              endforeach(OGGINC ${CMAKE_SYSTEM_PREFIX_PATH})
              message(">>> OGG VORBIS support enabled")
            endif(EXISTS ${VORBISFILE_LIB})
          endif(OGG_FOUND)
        endif(WITH_SND_OGG)

        if(WITH_SND_FLAC)
          pkg_check_modules(FLAC flac)
          if(FLAC_FOUND)
            find_library(FLAC_LIB NAMES FLAC)
            if(EXISTS ${FLAC_LIB})
              add_definitions(-DENGINE_SND_FLAC ${FLAC_CFLAGS})
              target_link_libraries(${P} ${FLAC_LIB})
              message(">>> FLAC support enabled")
            endif(EXISTS ${FLAC_LIB})
          endif(FLAC_FOUND)
        endif(WITH_SND_FLAC)

      endif(PKG_CONFIG_FOUND)
    endif(WITH_SND_OGG OR WITH_SND_FLAC)

  endif(OPENAL_FOUND)
endif(WITH_SND)
