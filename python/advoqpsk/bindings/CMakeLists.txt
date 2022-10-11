# Copyright 2020 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# SPDX-License-Identifier: GPL-3.0-or-later
#

########################################################################
# Check if there is C++ code at all
########################################################################
if(NOT advoqpsk_sources)
    MESSAGE(STATUS "No C++ sources... skipping python bindings")
    return()
endif(NOT advoqpsk_sources)

########################################################################
# Check for pygccxml
########################################################################
GR_PYTHON_CHECK_MODULE_RAW(
    "pygccxml"
    "import pygccxml"
    PYGCCXML_FOUND
    )

include(GrPybind)

########################################################################
# Python Bindings
########################################################################
list(APPEND advoqpsk_python_files
    precfofix_python.cc
    trigger_python.cc
    sync_python.cc
    decode_python.cc
    divider_python.cc
    powerstep_python.cc python_bindings.cc)

GR_PYBIND_MAKE_OOT(advoqpsk
   ../../..
   gr::advoqpsk
   "${advoqpsk_python_files}")

# copy bindings extension for use in QA test module
add_custom_command(TARGET advoqpsk_python POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:advoqpsk_python>
    ${PROJECT_BINARY_DIR}/test_modules/gnuradio/advoqpsk/
)

install(TARGETS advoqpsk_python DESTINATION ${GR_PYTHON_DIR}/gnuradio/advoqpsk COMPONENT pythonapi)
