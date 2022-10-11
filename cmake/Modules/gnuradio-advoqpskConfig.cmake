find_package(PkgConfig)

PKG_CHECK_MODULES(PC_GR_ADVOQPSK gnuradio-advoqpsk)

FIND_PATH(
    GR_ADVOQPSK_INCLUDE_DIRS
    NAMES gnuradio/advoqpsk/api.h
    HINTS $ENV{ADVOQPSK_DIR}/include
        ${PC_ADVOQPSK_INCLUDEDIR}
    PATHS ${CMAKE_INSTALL_PREFIX}/include
          /usr/local/include
          /usr/include
)

FIND_LIBRARY(
    GR_ADVOQPSK_LIBRARIES
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

include("${CMAKE_CURRENT_LIST_DIR}/gnuradio-advoqpskTarget.cmake")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GR_ADVOQPSK DEFAULT_MSG GR_ADVOQPSK_LIBRARIES GR_ADVOQPSK_INCLUDE_DIRS)
MARK_AS_ADVANCED(GR_ADVOQPSK_LIBRARIES GR_ADVOQPSK_INCLUDE_DIRS)
