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
/* BINDTOOL_HEADER_FILE(powerstep.h)                                        */
/* BINDTOOL_HEADER_FILE_HASH(e448cff339dbbc55b6f55fc97383e0bf)                     */
/***********************************************************************************/

#include <pybind11/complex.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

namespace py = pybind11;

#include <gnuradio/advoqpsk/powerstep.h>
// pydoc.h is automatically generated in the build directory
#include <powerstep_pydoc.h>

void bind_powerstep(py::module& m)
{

    using powerstep    = gr::advoqpsk::powerstep;


    py::class_<powerstep, gr::block, gr::basic_block,
        std::shared_ptr<powerstep>>(m, "powerstep", D(powerstep))

        .def(py::init(&powerstep::make),
           D(powerstep,make)
        )
        



        ;




}








