/*
 * Copyright 2022 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/***********************************************************************************/
/* This file is automatically generated using bindtool and can be manually edited  */
/* The following lines can be configured to regenerate this file during cmake      */
/* If manual edits are made, the following tags should be modified accordingly.    */
/* BINDTOOL_GEN_AUTOMATIC(0)                                                       */
/* BINDTOOL_USE_PYGCCXML(0)                                                        */
/* BINDTOOL_HEADER_FILE(divider.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(4a342cd6a7697e76345963f5a6af645c)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/advoqpsk/divider.h>
// pydoc.h is automatically generated in the build directory
#include <divider_pydoc.h>

void bind_divider(py::module& m)
{

    using divider    = gr::advoqpsk::divider;


    py::class_<divider, gr::sync_block, gr::block, gr::basic_block,
        std::shared_ptr<divider>>(m, "divider", D(divider))

        .def(py::init(&divider::make),
           D(divider,make)
        )
        



        ;




}








