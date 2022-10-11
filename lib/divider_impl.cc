/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "divider_impl.h"

namespace gr {
  namespace advoqpsk {
    divider::sptr
    divider::make()
    {
      return gnuradio::make_block_sptr<divider_impl>(
        );
    }


    /*
     * The private constructor
     */
    divider_impl::divider_impl()
      : gr::sync_block("divider",
              gr::io_signature::make(1 , 1, sizeof(gr_complex)),
              gr::io_signature::makev(2 , 2, std::vector<int>{sizeof(gr_complex), sizeof(float)}))
    {}

    /*
     * Our virtual destructor.
     */
    divider_impl::~divider_impl()
    {
    }

    int
    divider_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex* inSig = static_cast<const gr_complex*>(input_items[0]);
      gr_complex* outSig = static_cast<gr_complex*>(output_items[0]);
      float* outPower = static_cast<float*>(output_items[1]);

      for (int i = 0; i < noutput_items; i++)
      {
        outSig[i] = inSig[i];
        outPower[i] = inSig[i].real() * inSig[i].real() + inSig[i].imag() * inSig[i].imag();
      }

      // Tell runtime system how many output items we produced.
      return noutput_items;
    }

  } /* namespace advoqpsk */
} /* namespace gr */
