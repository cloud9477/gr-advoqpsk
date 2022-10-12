/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "precfofix_impl.h"

namespace gr {
  namespace advoqpsk {

    precfofix::sptr
    precfofix::make(int cfoin)
    {
      return gnuradio::make_block_sptr<precfofix_impl>(cfoin
        );
    }


    /*
     * The private constructor
     */
    precfofix_impl::precfofix_impl(int cfoin)
      : gr::sync_block("precfofix",
              gr::io_signature::make(1, 1, sizeof(gr_complex)),
              gr::io_signature::make(2, 2, sizeof(gr_complex))),
              d_cfo(cfoin)
    {
      d_radNum = d_cfo;
      d_radStep = 2.0f * M_PI / ((float)d_radNum);
      d_radPos = new gr_complex[d_radNum];
      d_radNeg = new gr_complex[d_radNum];

      for (int i = 0; i < d_radNum; i++)
      {
        d_radPos[i] = gr_complex(cosf(d_radStep * (float)i),  sinf(d_radStep * (float)i));
        d_radNeg[i] = gr_complex(cosf(d_radStep * (float)i), -sinf(d_radStep * (float)i));
      }
      d_radCounter = 0;
    }

    /*
     * Our virtual destructor.
     */
    precfofix_impl::~precfofix_impl()
    {
      delete[] d_radPos;
      delete[] d_radNeg;
      d_radPos = NULL;
      d_radNeg = NULL;
    }

    int
    precfofix_impl::work(int noutput_items,
        gr_vector_const_void_star &input_items,
        gr_vector_void_star &output_items)
    {
      const gr_complex* in = static_cast<const gr_complex*>(input_items[0]);
      gr_complex* out1 = static_cast<gr_complex*>(output_items[0]);
      gr_complex* out2 = static_cast<gr_complex*>(output_items[0]);

      for (int i = 0; i < noutput_items; i++)
      {
        out1[i] = in[i] * d_radPos[d_radCounter];
        out2[i] = in[i] * d_radNeg[d_radCounter];
        d_radCounter++;
        if (d_radCounter >= d_radNum)
        {
          d_radCounter = 0;
        }
      }

      return noutput_items;
    }

  } /* namespace advoqpsk */
} /* namespace gr */
