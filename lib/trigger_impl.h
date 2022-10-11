/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ADVOQPSK_TRIGGER_IMPL_H
#define INCLUDED_ADVOQPSK_TRIGGER_IMPL_H

#include <gnuradio/advoqpsk/trigger.h>

namespace gr {
  namespace advoqpsk {

    class trigger_impl : public trigger
    {
     private:
      // Nothing to declare in this block.

     public:
      trigger_impl();
      ~trigger_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

    };

  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_TRIGGER_IMPL_H */
