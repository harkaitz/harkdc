# #
include_guard()
if (ENABLE_STATIC OR WIN32)
  find_library(ZLIB_LIBRARIES NAMES libz.a libz.lib)
else()
  find_library(ZLIB_LIBRARIES z)
endif()
message("++ ZLIB: ${ZLIB_LIBRARIES}")
list(APPEND LIBS ${ZLIB_LIBRARIES})
