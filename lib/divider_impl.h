/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ADVOQPSK_DIVIDER_IMPL_H
#define INCLUDED_ADVOQPSK_DIVIDER_IMPL_H

#include <gnuradio/advoqpsk/divider.h>

namespace gr {
  namespace advoqpsk {

    class divider_impl : public divider
    {
     private:

     public:
      divider_impl();
      ~divider_impl();

      // Where all the action really happens
      int work(
              int noutput_items,
              gr_vector_const_void_star &input_items,
              gr_vector_void_star &output_items
      );
    };

  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_DIVIDER_IMPL_H */
