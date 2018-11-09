# #
include_guard()
find_library(serialport_LIBRARY NAMES libserialport.a libserialport.lib)
find_path(serialport_INCLUDE libserialport.h)
if (NOT (serialport_LIBRARY AND serialport_INCLUDE))
  message(FATAL_ERROR "Library `serialport` not found in; ${CMAKE_PREFIX_PATH}")
endif()
message("++ serialport: ${serialport_INCLUDE} ${serialport_LIBRARY}")
list(APPEND INCLUDES ${serialport_INCLUDE})
if (WIN32)
  list(APPEND LIBS setupapi user32 ${serialport_LIBRARY})
else()
  list(APPEND LIBS ${serialport_LIBRARY})
endif()



