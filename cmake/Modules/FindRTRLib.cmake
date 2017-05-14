include(FindPackageHandleStandardArgs)

find_path(RTRLIB_INCLUDE_DIR rtrlib/rtrlib.h)
find_library(RTRLIB_LIBRARY NAMES rtr)

find_package_handle_standard_args(RTRLib DEFAULT_MSG RTRLIB_LIBRARY RTRLIB_INCLUDE_DIR)

mark_as_advanced(RTRLIB_INCLUDE_DIR RTRLIB_LIBRARY)

set(RTRLIB_LIBRARIES ${RTRLIB_LIBRARY})
set(RTRLIB_INCLUDE_DIRS ${RTRLIB_INCLUDE_DIR})

