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
#include "advoqpsk_precfofix_impl.h"

namespace gr
{
  namespace advoqpsk
  {
    advoqpsk_precfofix::sptr
    advoqpsk_precfofix::make(int cfoin)
    {
      return gnuradio::get_initial_sptr(new advoqpsk_precfofix_impl(cfoin));
    }

    /*
     * The private constructor
     */
    static int ios[] = {sizeof(gr_complexd), sizeof(gr_complexd)};
    static std::vector<int> iosig(ios, ios + sizeof(ios) / sizeof(int));
    advoqpsk_precfofix_impl::advoqpsk_precfofix_impl(int cfoin)
        : gr::sync_block("advoqpsk_precfofix",
                         gr::io_signature::make(1, 1, sizeof(gr_complexd)),
                         gr::io_signature::makev(2, 2, iosig)),
          d_cfo(cfoin)
    {
      d_radNum = d_cfo;
      d_radStep = 2.0 * M_PI / ((double)d_radNum);
      d_radPos = new gr_complexd[d_radNum];
      d_radNeg = new gr_complexd[d_radNum];

      for (int i = 0; i < d_radNum; i++)
      {
        d_radPos[i] = gr_complexd(cos(d_radStep * (double)i), sin(d_radStep * (double)i));
        d_radNeg[i] = gr_complexd(cos(d_radStep * (double)i), -sin(d_radStep * (double)i));
      }
      d_radCounter = 0;
    }

    /*
     * Our virtual destructor.
     */
    advoqpsk_precfofix_impl::~advoqpsk_precfofix_impl()
    {
      delete[] d_radPos;
      delete[] d_radNeg;
      d_radPos = NULL;
      d_radNeg = NULL;
    }

    int
    advoqpsk_precfofix_impl::work(int noutput_items,
                                  gr_vector_const_void_star &input_items,
                                  gr_vector_void_star &output_items)
    {
      const gr_complexd *in = (const gr_complexd *)input_items[0];
      gr_complexd *outPos = (gr_complexd *)output_items[0];
      gr_complexd *outNeg = (gr_complexd *)output_items[1];

      for (int i = 0; i < noutput_items; i++)
      {
        outPos[i] = in[i] * d_radPos[d_radCounter];
        outNeg[i] = in[i] * d_radNeg[d_radCounter];
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
