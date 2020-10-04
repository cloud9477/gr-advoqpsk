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
        : gr::sync_block("advoqpsk_powerstep",
                         gr::io_signature::make(1, 1, sizeof(double)),
                         gr::io_signature::make(1, 1, sizeof(double)))
    {
      d_bufHeadLen = 128;
      d_bufMaxLen = 8192;
      d_pBuf = new double[d_bufMaxLen];
      for (int i = 0; i < d_bufHeadLen; i++)
      {
        d_pBuf[i] = 2.0;
      }
    }

    /*
     * Our virtual destructor.
     */
    advoqpsk_powerstep_impl::~advoqpsk_powerstep_impl()
    {
      delete[] d_pBuf;
      d_pBuf = NULL;
    }

    int
    advoqpsk_powerstep_impl::work(int noutput_items,
                                  gr_vector_const_void_star &input_items,
                                  gr_vector_void_star &output_items)
    {
      const double *in = (const double *)input_items[0];
      double *out = (double *)output_items[0];
      memcpy(&d_pBuf[d_bufHeadLen], in, noutput_items * sizeof(double));

      for (int i = 0; i < noutput_items; i++)
      {
        out[i] = 0.0;
        for (int j = 0; j < 64; j += 2)
        {
          out[i] += d_pBuf[i + j];
        }
      }

      memcpy(&d_pBuf[0], &d_pBuf[noutput_items], d_bufHeadLen * sizeof(double));
      
      return noutput_items;
    }
  } /* namespace advoqpsk */
} /* namespace gr */
