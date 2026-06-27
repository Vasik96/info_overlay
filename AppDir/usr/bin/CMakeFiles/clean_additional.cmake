# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles/info_overlay_bin_autogen.dir/AutogenUsed.txt"
  "CMakeFiles/info_overlay_bin_autogen.dir/ParseCache.txt"
  "info_overlay_bin_autogen"
  )
endif()
