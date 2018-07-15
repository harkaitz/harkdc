## -- Dependency: oslibs
if (WIN32)
  find_library(ZLIB NAMES libz.a libz.lib)
  find_library(PTHREAD NAMES libpthread.a)
  set(OSLIBS ${ZLIB} ${PTHREAD} setupapi user32)
else()
  find_library(ZLIB NAMES libz.so)
  set(OSLIBS ${ZLIB} pthread)
endif()
add_library(oslibs INTERFACE)
target_link_libraries(oslibs INTERFACE ${OSLIBS})
message("** OSLIBS: ${OSLIBS}")



## -- Dependency: uterm
find_library(uterm_LIBRARY NAMES libuterm.a libuterm.lib)
find_path(uterm_INCLUDE uterm/uterm.h)
if (uterm_LIBRARY AND uterm_INCLUDE)
  message("** uterm: ${uterm_INCLUDE} ${uterm_LIBRARY}")
  add_library(uterm UNKNOWN IMPORTED GLOBAL)
  set_target_properties(uterm PROPERTIES
    IMPORTED_LOCATION ${uterm_LIBRARY}
    INTERFACE_INCLUDE_DIRECTORIES ${uterm_INCLUDE})
else()
  message(FATAL_ERROR "Library `uterm` not found in; ${CMAKE_PREFIX_PATH}")
endif()

## -- Dependency: serialport
find_library(serialport_LIBRARY NAMES libserialport.a libserialport.lib)
find_path(serialport_INCLUDE libserialport.h)
if (serialport_LIBRARY AND serialport_INCLUDE)
  message("** serialport: ${serialport_INCLUDE} ${serialport_LIBRARY}")
  add_library(serialport UNKNOWN IMPORTED GLOBAL)
  set_target_properties(serialport PROPERTIES
    IMPORTED_LOCATION ${serialport_LIBRARY}
    INTERFACE_INCLUDE_DIRECTORIES ${serialport_INCLUDE})
else()
  message(FATAL_ERROR "Library `serialport` not found in; ${CMAKE_PREFIX_PATH}")
endif()

## -- Dependency: xlsxwriter
find_library(xlsxwriter_LIBRARY NAMES libxlsxwriter.a libxlsxwriter.lib)
find_path(xlsxwriter_INCLUDE xlsxwriter.h)
if (xlsxwriter_LIBRARY AND xlsxwriter_INCLUDE)
  message("** xlsxwriter: ${xlsxwriter_INCLUDE} ${xlsxwriter_LIBRARY}")
  add_library(xlsxwriter UNKNOWN IMPORTED GLOBAL)
  set_target_properties(xlsxwriter PROPERTIES
    IMPORTED_LOCATION ${xlsxwriter_LIBRARY}
    INTERFACE_INCLUDE_DIRECTORIES ${xlsxwriter_INCLUDE})
else()
  message(FATAL_ERROR "Library `xlsxwriter` not found in; ${CMAKE_PREFIX_PATH}")
endif()
