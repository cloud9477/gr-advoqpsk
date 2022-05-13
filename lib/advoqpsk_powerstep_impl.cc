/* -*- c++ -*- */
/* 
 * Copyright 2020 Zelin Yun.
 * 
 * This is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 * 
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this software; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <gnuradio/io_signature.h>
#include "advoqpsk_powerstep_impl.h"

namespace gr
{
  namespace advoqpsk
  {
    advoqpsk_powerstep::sptr
    advoqpsk_powerstep::make()
    {
      return gnuradio::get_initial_sptr(new advoqpsk_powerstep_impl());
    }

    /*
     * The private constructor
     */
    advoqpsk_powerstep_impl::advoqpsk_powerstep_impl()
        : gr::block("advoqpsk_powerstep",
                         gr::io_signature::make(1, 1, sizeof(double)),
                         gr::io_signature::make(1, 1, sizeof(double)))
    {
    }

    /*
     * Our virtual destructor.
     */
    advoqpsk_powerstep_impl::~advoqpsk_powerstep_impl()
    {
    }

    void
    advoqpsk_powerstep_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      gr_vector_int::size_type ninputs = ninput_items_required.size();
      for(int i=0; i < ninputs; i++)
      {
	      ninput_items_required[i] = noutput_items + 128;
      }
    }

    int
    advoqpsk_powerstep_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const double *in = (const double *) input_items[0];
      double *out = (double *) output_items[0];
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
