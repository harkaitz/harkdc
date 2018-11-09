# @HPROJ_UPDATE@
include_guard()
find_library(TCL_MODULE_ENABLE NAMES libtclstub8.6.a libtclstub8.6.lib)
if (TCL_MODULE_ENABLE)
  message("++ TCL_MODULE_ENABLE=${TCL_MODULE_ENABLE}")
else()
  message("%% Can't find libtclstub8.6.a, TCL modules disabled. TCL_MODULE_ENABLE=${TCL_MODULE_ENABLE}")
endif()
