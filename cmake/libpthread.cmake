# #
include_guard()
if (WIN32)
  find_library(PTHREAD_LIBRARIES NAMES libpthread.a libpthread.lib)
  if (NOT PTHREAD_LIBRARIES)
    message(SEND_ERROR "PTHREAD library not found")
  endif(NOT PTHREAD_LIBRARIES)
  message("++ PTHREAD: ${PTHREAD_LIBRARIES}")
  list(APPEND LIBS ${PTHREAD_LIBRARIES})
else()
  set(CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -pthread")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
  set(CMAKE_LD_FLAGS  "${CMAKE_LD_FLAGS} -pthread")
  message("++ PTHREAD: -pthread")
endif()


