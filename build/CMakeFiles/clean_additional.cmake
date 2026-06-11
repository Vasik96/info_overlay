# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "src/CMakeFiles/info_overlay_bin_autogen.dir/AutogenUsed.txt"
  "src/CMakeFiles/info_overlay_bin_autogen.dir/ParseCache.txt"
  "src/info_overlay_bin_autogen"
  )
endif()
