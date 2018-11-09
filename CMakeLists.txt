cmake_minimum_required(VERSION 3.11)
project(harkdc VERSION 0.1)
find_package(PkgConfig REQUIRED)

# C/C++ flags.
if ( CMAKE_COMPILER_IS_GNUCC )
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -Werror -std=gnu99")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -std=c++11")
  if(${CMAKE_SYSTEM_NAME} STREQUAL Windows)
    set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -static")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static")
  endif()
endif ( CMAKE_COMPILER_IS_GNUCC )
if ( MSVC )
  set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif ( MSVC )

# Source all .cmake files.
file(GLOB cmakefiles "${CMAKE_SOURCE_DIR}/cmake/*.cmake")
foreach(cmakefile ${cmakefiles})
  include(${cmakefile})
endforeach()
list(REVERSE LIBS)


# Package config.
message(cmake/${PROJECT_NAME}-config.cmake.in)
if(EXISTS ${CMAKE_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in)
  include(CMakePackageConfigHelpers)
  configure_package_config_file(
    cmake/${PROJECT_NAME}-config.cmake.in
    ${PROJECT_NAME}-config.cmake
    INSTALL_DESTINATION lib/${PROJECT_NAME}-${PROJECT_VERSION})
  write_basic_package_version_file(
    ${PROJECT_NAME}-config-version.cmake
    VERSION       ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion)
  install(
    FILES
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-config.cmake
    ${CMAKE_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
    DESTINATION lib/${PROJECT_NAME}-${PROJECT_VERSION})
endif()

# 
if (EXISTS ${CMAKE_SOURCE_DIR}/src/CMakeLists.txt)
  add_subdirectory(${CMAKE_SOURCE_DIR}/src)
endif()
