## -- HARKD LIBRARY.
file(GLOB harkd_DRIVER_SOURCES devices/*.c )
file(GLOB harkd_TEST_SOURCES   tests/*.c   )

add_custom_command(
  OUTPUT tests/tests.h
  MAIN_DEPENDENCY makeh
  DEPENDS ${harkd_TEST_SOURCES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  COMMAND sh makeh "const harkd_test_t" harkd_tests ${harkd_TEST_SOURCES} > tests/tests.h)

add_custom_command(
  OUTPUT devices/devices.h
  MAIN_DEPENDENCY makeh
  DEPENDS ${harkd_DRIVER_SOURCES}
  WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
  COMMAND sh makeh "const harkd_def_t" harkd_device_list ${harkd_DRIVER_SOURCES} > devices/devices.h)

set(harkd_SOURCES
  harkd.c
  tests/tests.h
  devices/devices.h
  ${harkd_DRIVER_SOURCES}
  ${harkd_TEST_SOURCES}
  )
set(harkd_HEADERS
  harkd.h harkd-device.h
  )

add_library(harkd STATIC ${harkd_SOURCES} ${harkd_HEADERS})
install(TARGETS harkd DESTINATION lib)
install(FILES ${harkd_HEADERS} DESTINATION include/harkd)
target_link_libraries(harkd uterm serialport xlsxwriter oslibs)

## -- HARKDC command line interface.
if (WIN32)
  add_executable(harkdc main.c harkd.rc)
else()
  add_executable(harkdc main.c)
endif()
target_link_libraries(harkdc harkd)
install(TARGETS harkdc DESTINATION bin)
