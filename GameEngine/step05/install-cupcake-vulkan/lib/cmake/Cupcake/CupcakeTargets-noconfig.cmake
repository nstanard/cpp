#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "Cupcake::Cupcake" for configuration ""
set_property(TARGET Cupcake::Cupcake APPEND PROPERTY IMPORTED_CONFIGURATIONS NOCONFIG)
set_target_properties(Cupcake::Cupcake PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_NOCONFIG "CXX"
  IMPORTED_LOCATION_NOCONFIG "${_IMPORT_PREFIX}/lib/libCupcake.a"
  )

list(APPEND _cmake_import_check_targets Cupcake::Cupcake )
list(APPEND _cmake_import_check_files_for_Cupcake::Cupcake "${_IMPORT_PREFIX}/lib/libCupcake.a" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
