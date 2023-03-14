#----------------------------------------------------------------
# Generated CMake target import file for configuration "RelWithDebInfo".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "hdf5::szip-static" for configuration "RelWithDebInfo"
set_property(TARGET hdf5::szip-static APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(hdf5::szip-static PROPERTIES
  IMPORTED_LINK_INTERFACE_LANGUAGES_RELWITHDEBINFO "C"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/libszip.lib"
  )

list(APPEND _IMPORT_CHECK_TARGETS hdf5::szip-static )
list(APPEND _IMPORT_CHECK_FILES_FOR_hdf5::szip-static "${_IMPORT_PREFIX}/lib/libszip.lib" )

# Import target "hdf5::szip-shared" for configuration "RelWithDebInfo"
set_property(TARGET hdf5::szip-shared APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)
set_target_properties(hdf5::szip-shared PROPERTIES
  IMPORTED_IMPLIB_RELWITHDEBINFO "${_IMPORT_PREFIX}/lib/szip.lib"
  IMPORTED_LOCATION_RELWITHDEBINFO "${_IMPORT_PREFIX}/bin/szip.dll"
  )

list(APPEND _IMPORT_CHECK_TARGETS hdf5::szip-shared )
list(APPEND _IMPORT_CHECK_FILES_FOR_hdf5::szip-shared "${_IMPORT_PREFIX}/lib/szip.lib" "${_IMPORT_PREFIX}/bin/szip.dll" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
