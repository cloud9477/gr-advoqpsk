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
#include "advoqpsk_divider_impl.h"

namespace gr
{
  namespace advoqpsk
  {
    advoqpsk_divider::sptr
    advoqpsk_divider::make()
    {
      return gnuradio::get_initial_sptr(new advoqpsk_divider_impl());
    }

    /*
     * The private constructor
     */
    static int ios[] = {sizeof(gr_complexd), sizeof(double)};
    static std::vector<int> iosig(ios, ios + sizeof(ios) / sizeof(int));
    advoqpsk_divider_impl::advoqpsk_divider_impl()
        : gr::sync_block("advoqpsk_divider",
                         gr::io_signature::make(1, 1, sizeof(gr_complex)),
                         gr::io_signature::makev(2, 2, iosig))
    {
    }

    /*
     * Our virtual destructor.
     */
    advoqpsk_divider_impl::~advoqpsk_divider_impl()
    {
    }

    int
    advoqpsk_divider_impl::work(int noutput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items)
    {
      const gr_complex *in = (const gr_complex *)input_items[0];
      gr_complexd *outSig = (gr_complexd *)output_items[0];
      double *outSq = (double *)output_items[1];
      double d_real;
      double d_imag;

      for (int i = 0; i < noutput_items; i++)
      {
        d_real = (double)in[i].real();
        d_imag = (double)in[i].imag();
        outSig[i] = gr_complexd(d_real, d_imag);
        outSq[i] = d_real * d_real + d_imag * d_imag;
      }

      return noutput_items;
    }
  } /* namespace advoqpsk */
} /* namespace gr */
