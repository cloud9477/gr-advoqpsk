/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ADVOQPSK_PRECFOFIX_IMPL_H
#define INCLUDED_ADVOQPSK_PRECFOFIX_IMPL_H

#include <gnuradio/advoqpsk/precfofix.h>

namespace gr {
  namespace advoqpsk {

    class precfofix_impl : public precfofix
    {
     private:
      // Nothing to declare in this block.

     public:
      precfofix_impl();
      ~precfofix_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_PRECFOFIX_IMPL_H */
