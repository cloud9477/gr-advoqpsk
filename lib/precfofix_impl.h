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
      int d_cfo;
      float d_radStep;
      gr_complex *d_radPos;
      gr_complex *d_radNeg;
      int d_radNum;
      int d_radCounter;

     public:
      precfofix_impl(int cfoin);
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
