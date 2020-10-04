INCLUDE(FindPkgConfig)
PKG_CHECK_MODULES(PC_ADVOQPSK advoqpsk)

FIND_PATH(
    ADVOQPSK_INCLUDE_DIRS
    NAMES advoqpsk/api.h
    HINTS $ENV{ADVOQPSK_DIR}/include
        ${PC_ADVOQPSK_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    ADVOQPSK_LIBRARIES
    NAMES gnuradio-advoqpsk
    HINTS $ENV{ADVOQPSK_DIR}/lib
        ${PC_ADVOQPSK_LIBDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/lib
          ${CMAKE_INSTALL_PREFIX}/lib64
          /usr/local/lib
          /usr/local/lib64
          /usr/lib
          /usr/lib64
)

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(ADVOQPSK DEFAULT_MSG ADVOQPSK_LIBRARIES ADVOQPSK_INCLUDE_DIRS)
MARK_AS_ADVANCED(ADVOQPSK_LIBRARIES ADVOQPSK_INCLUDE_DIRS)

