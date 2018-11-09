# #
include_guard()
find_library(HITZ_LIBRARIES NAMES libhitz.a libhitz.lib)
if (NOT HITZ_LIBRARIES)
  message(SEND_ERROR "HITZ library not found")
endif(NOT HITZ_LIBRARIES)
message("++ HITZ: ${HITZ_LIBRARIES}")
list(APPEND LIBS ${HITZ_LIBRARIES})
