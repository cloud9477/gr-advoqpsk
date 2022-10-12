/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "powerstep_impl.h"

namespace gr {
  namespace advoqpsk {

    powerstep::sptr
    powerstep::make()
    {
      return gnuradio::make_block_sptr<powerstep_impl>(
        );
    }


    /*
     * The private constructor
     */
    powerstep_impl::powerstep_impl()
      : gr::block("powerstep",
              gr::io_signature::make(1, 1, sizeof(float)),
              gr::io_signature::make(1, 1, sizeof(float)))
    {}

    /*
     * Our virtual destructor.
     */
    powerstep_impl::~powerstep_impl()
    {
    }

    void
    powerstep_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items + 128;
    }

    int
    powerstep_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const float* in = static_cast<const float*>(input_items[0]);
      float* out = static_cast<float*>(output_items[0]);

      int nSampProc = 0;   // number of consumed input samples and generated output samples
      int nInputLimit = ninput_items[0]-128;   // number of limited input samples can be used
      if(nInputLimit > 0)
      {
        while(nSampProc < noutput_items && nSampProc < nInputLimit)
        {
          out[nSampProc] = 0.0f;
          for(int i=0;i<64;i+=2)
          {
            out[nSampProc] += in[i+nSampProc];
          }
          nSampProc++;
        }
      }

      // Tell runtime system how many input smaples consumed.
      consume_each (nSampProc);
      // Tell runtime system how many output items we produced.
      return nSampProc;
    }

  } /* namespace advoqpsk */
} /* namespace gr */
