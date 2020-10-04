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
#include "advoqpsk_trigger_impl.h"

namespace gr
{
  namespace advoqpsk
  {
    advoqpsk_trigger::sptr
    advoqpsk_trigger::make(float ccThreshold)
    {
      return gnuradio::get_initial_sptr(new advoqpsk_trigger_impl(ccThreshold));
    }

    /*
     * The private constructor
     */
    int iosA7ZzIn[] = {sizeof(gr_complexd), sizeof(double)};
    std::vector<int> iosigA7ZzIn(iosA7ZzIn, iosA7ZzIn + sizeof(iosA7ZzIn) / sizeof(int));
    int iosA7ZzOut[] = {sizeof(int)};
    std::vector<int> iosigA7ZzOut(iosA7ZzOut, iosA7ZzOut + sizeof(iosA7ZzOut) / sizeof(int));
    advoqpsk_trigger_impl::advoqpsk_trigger_impl(float ccThreshold)
        : gr::sync_block("advoqpsk_trigger",
                         gr::io_signature::makev(2, 2, iosigA7ZzIn),
                         gr::io_signature::makev(1, 1, iosigA7ZzOut)),
          d_ccThreshold(ccThreshold)
    {
      d_bufHeadLen = 512; /* 4 bytes */
      d_bufMaxLen = 8192;           /* 2ms */
      d_pBuf = new gr_complexd[d_bufMaxLen];
      d_pBufPower = new double[d_bufMaxLen];
      d_pBufCc7 = new int[d_bufMaxLen];
      d_pBufCcZ = new int[d_bufMaxLen];
      for (int i = 0; i < d_bufHeadLen; i++)
      {
        d_pBuf[i] = gr_complexd(1.0, 1.0);
        d_pBufPower[i] = 256.0;
        d_pBufCc7[i] = 0;
        d_pBufCcZ[i] = 0;
      }
      d_ccThreshold = d_ccThreshold * d_ccThreshold * 32.0;
    }

    /*
     * Our virtual destructor.
     */
    advoqpsk_trigger_impl::~advoqpsk_trigger_impl()
    {
      delete[] d_pBuf;
      delete[] d_pBufPower;
      delete[] d_pBufCc7;
      delete[] d_pBufCcZ;
      d_pBuf = NULL;
      d_pBufPower = NULL;
      d_pBufCc7 = NULL;
      d_pBufCcZ = NULL;
    }

    int
    advoqpsk_trigger_impl::work(int noutput_items,
                                gr_vector_const_void_star &input_items,
                                gr_vector_void_star &output_items)
    {
      const gr_complexd *in0 = (const gr_complexd *)input_items[0];
      double *in1 = (double *)input_items[1];
      int *out = (int *)output_items[0];

      memcpy(&d_pBuf[d_bufHeadLen], in0, noutput_items * sizeof(gr_complexd));
      memcpy(&d_pBufPower[d_bufHeadLen], in1, noutput_items * sizeof(double));

      for (int i = 0; i < noutput_items; i++)
      {
        if (fCcZ(&d_pBuf[i + 384], d_pBufPower[i + 384]) > d_ccThreshold)
        {
          d_pBufCcZ[i + 384] = 1;
        }
        else
        {
          d_pBufCcZ[i + 384] = 0;
        }
        if (fCc7(&d_pBuf[i + 384], d_pBufPower[i + 384]) > d_ccThreshold)
        {
          d_pBufCc7[i + 384] = 1;
        }
        else
        {
          d_pBufCc7[i + 384] = 0;
        }
        out[i] = 0;
        if (fCcA(&d_pBuf[i + 448], d_pBufPower[i + 448]) > d_ccThreshold)
        {
          if (d_pBufCc7[i + 384] && d_pBufCcZ[i + 320] && d_pBufCcZ[i + 256] && d_pBufCcZ[i + 192] && d_pBufCcZ[i + 128] && d_pBufCcZ[i + 64] && d_pBufCcZ[i])
          {
            out[i] = 1;
          }
        }
      }

      memcpy(&d_pBufCc7[0], &d_pBufCc7[noutput_items], d_bufHeadLen * sizeof(int));
      memcpy(&d_pBufCcZ[0], &d_pBufCcZ[noutput_items], d_bufHeadLen * sizeof(int));
      memcpy(&d_pBuf[0], &d_pBuf[noutput_items], d_bufHeadLen * sizeof(gr_complexd));
      memcpy(&d_pBufPower[0], &d_pBufPower[noutput_items], d_bufHeadLen * sizeof(double));

      return noutput_items;
    }

    double
    advoqpsk_trigger_impl::fCc7(gr_complexd *p_sigIn, double p_sigPower)
    {
      double d_ccReal1 = 0.0;
      double d_ccReal2 = 0.0;
      double d_ccImag1 = 0.0;
      double d_ccImag2 = 0.0;

      d_ccReal1 += p_sigIn[2].real();
      d_ccImag1 += p_sigIn[2].imag();
      d_ccReal2 -= p_sigIn[6].real();
      d_ccImag2 -= p_sigIn[6].imag();
      d_ccReal1 += p_sigIn[10].real();
      d_ccImag1 += p_sigIn[10].imag();
      d_ccReal2 -= p_sigIn[14].real();
      d_ccImag2 -= p_sigIn[14].imag();
      d_ccReal1 -= p_sigIn[18].real();
      d_ccImag1 -= p_sigIn[18].imag();
      d_ccReal2 += p_sigIn[22].real();
      d_ccImag2 += p_sigIn[22].imag();
      d_ccReal1 -= p_sigIn[26].real();
      d_ccImag1 -= p_sigIn[26].imag();
      d_ccReal2 -= p_sigIn[30].real();
      d_ccImag2 -= p_sigIn[30].imag();
      d_ccReal1 -= p_sigIn[34].real();
      d_ccImag1 -= p_sigIn[34].imag();
      d_ccReal2 += p_sigIn[38].real();
      d_ccImag2 += p_sigIn[38].imag();
      d_ccReal1 -= p_sigIn[42].real();
      d_ccImag1 -= p_sigIn[42].imag();
      d_ccReal2 += p_sigIn[46].real();
      d_ccImag2 += p_sigIn[46].imag();
      d_ccReal1 += p_sigIn[50].real();
      d_ccImag1 += p_sigIn[50].imag();
      d_ccReal2 += p_sigIn[54].real();
      d_ccImag2 += p_sigIn[54].imag();
      d_ccReal1 += p_sigIn[58].real();
      d_ccImag1 += p_sigIn[58].imag();
      d_ccReal2 -= p_sigIn[62].real();
      d_ccImag2 -= p_sigIn[62].imag();

      d_ccReal1 -= p_sigIn[0].imag();
      d_ccImag1 += p_sigIn[0].real();
      d_ccReal2 -= p_sigIn[4].imag();
      d_ccImag2 += p_sigIn[4].real();
      d_ccReal1 += p_sigIn[8].imag();
      d_ccImag1 -= p_sigIn[8].real();
      d_ccReal2 += p_sigIn[12].imag();
      d_ccImag2 -= p_sigIn[12].real();
      d_ccReal1 -= p_sigIn[16].imag();
      d_ccImag1 += p_sigIn[16].real();
      d_ccReal2 -= p_sigIn[20].imag();
      d_ccImag2 += p_sigIn[20].real();
      d_ccReal1 += p_sigIn[24].imag();
      d_ccImag1 -= p_sigIn[24].real();
      d_ccReal2 += p_sigIn[28].imag();
      d_ccImag2 -= p_sigIn[28].real();
      d_ccReal1 += p_sigIn[32].imag();
      d_ccImag1 -= p_sigIn[32].real();
      d_ccReal2 -= p_sigIn[36].imag();
      d_ccImag2 += p_sigIn[36].real();
      d_ccReal1 -= p_sigIn[40].imag();
      d_ccImag1 += p_sigIn[40].real();
      d_ccReal2 -= p_sigIn[44].imag();
      d_ccImag2 += p_sigIn[44].real();
      d_ccReal1 -= p_sigIn[48].imag();
      d_ccImag1 += p_sigIn[48].real();
      d_ccReal2 += p_sigIn[52].imag();
      d_ccImag2 -= p_sigIn[52].real();
      d_ccReal1 -= p_sigIn[56].imag();
      d_ccImag1 += p_sigIn[56].real();
      d_ccReal2 += p_sigIn[60].imag();
      d_ccImag2 -= p_sigIn[60].real();

      d_ccReal1 = d_ccReal1 + d_ccReal2;
      d_ccImag1 = d_ccImag1 + d_ccImag2;

      return ((d_ccReal1 * d_ccReal1 + d_ccImag1 * d_ccImag1) / p_sigPower);
    }

    double
    advoqpsk_trigger_impl::fCcA(gr_complexd *p_sigIn, double p_sigPower)
    {
      double d_ccReal1 = 0.0;
      double d_ccReal2 = 0.0;
      double d_ccImag1 = 0.0;
      double d_ccImag2 = 0.0;

      d_ccReal1 -= p_sigIn[2].real();
      d_ccImag1 -= p_sigIn[2].imag();
      d_ccReal2 += p_sigIn[6].real();
      d_ccImag2 += p_sigIn[6].imag();
      d_ccReal1 += p_sigIn[10].real();
      d_ccImag1 += p_sigIn[10].imag();
      d_ccReal2 += p_sigIn[14].real();
      d_ccImag2 += p_sigIn[14].imag();
      d_ccReal1 += p_sigIn[18].real();
      d_ccImag1 += p_sigIn[18].imag();
      d_ccReal2 -= p_sigIn[22].real();
      d_ccImag2 -= p_sigIn[22].imag();
      d_ccReal1 += p_sigIn[26].real();
      d_ccImag1 += p_sigIn[26].imag();
      d_ccReal2 -= p_sigIn[30].real();
      d_ccImag2 -= p_sigIn[30].imag();
      d_ccReal1 += p_sigIn[34].real();
      d_ccImag1 += p_sigIn[34].imag();
      d_ccReal2 -= p_sigIn[38].real();
      d_ccImag2 -= p_sigIn[38].imag();
      d_ccReal1 -= p_sigIn[42].real();
      d_ccImag1 -= p_sigIn[42].imag();
      d_ccReal2 += p_sigIn[46].real();
      d_ccImag2 += p_sigIn[46].imag();
      d_ccReal1 -= p_sigIn[50].real();
      d_ccImag1 -= p_sigIn[50].imag();
      d_ccReal2 -= p_sigIn[54].real();
      d_ccImag2 -= p_sigIn[54].imag();
      d_ccReal1 -= p_sigIn[58].real();
      d_ccImag1 -= p_sigIn[58].imag();
      d_ccReal2 += p_sigIn[62].real();
      d_ccImag2 += p_sigIn[62].imag();

      d_ccReal1 += p_sigIn[0].imag();
      d_ccImag1 -= p_sigIn[0].real();
      d_ccReal2 += p_sigIn[4].imag();
      d_ccImag2 -= p_sigIn[4].real();
      d_ccReal1 += p_sigIn[8].imag();
      d_ccImag1 -= p_sigIn[8].real();
      d_ccReal2 -= p_sigIn[12].imag();
      d_ccImag2 += p_sigIn[12].real();
      d_ccReal1 += p_sigIn[16].imag();
      d_ccImag1 -= p_sigIn[16].real();
      d_ccReal2 -= p_sigIn[20].imag();
      d_ccImag2 += p_sigIn[20].real();
      d_ccReal1 -= p_sigIn[24].imag();
      d_ccImag1 += p_sigIn[24].real();
      d_ccReal2 += p_sigIn[28].imag();
      d_ccImag2 -= p_sigIn[28].real();
      d_ccReal1 -= p_sigIn[32].imag();
      d_ccImag1 += p_sigIn[32].real();
      d_ccReal2 -= p_sigIn[36].imag();
      d_ccImag2 += p_sigIn[36].real();
      d_ccReal1 += p_sigIn[40].imag();
      d_ccImag1 -= p_sigIn[40].real();
      d_ccReal2 += p_sigIn[44].imag();
      d_ccImag2 -= p_sigIn[44].real();
      d_ccReal1 -= p_sigIn[48].imag();
      d_ccImag1 += p_sigIn[48].real();
      d_ccReal2 -= p_sigIn[52].imag();
      d_ccImag2 += p_sigIn[52].real();
      d_ccReal1 -= p_sigIn[56].imag();
      d_ccImag1 += p_sigIn[56].real();
      d_ccReal2 += p_sigIn[60].imag();
      d_ccImag2 -= p_sigIn[60].real();

      d_ccReal1 = d_ccReal1 + d_ccReal2;
      d_ccImag1 = d_ccImag1 + d_ccImag2;

      return ((d_ccReal1 * d_ccReal1 + d_ccImag1 * d_ccImag1) / p_sigPower);
    }

    double
    advoqpsk_trigger_impl::fCcZ(gr_complexd *p_sigIn, double p_sigPower)
    {
      double d_ccReal1 = 0.0;
      double d_ccReal2 = 0.0;
      double d_ccImag1 = 0.0;
      double d_ccImag2 = 0.0;

      d_ccReal1 += p_sigIn[2].real();
      d_ccImag1 += p_sigIn[2].imag();
      d_ccReal2 -= p_sigIn[6].real();
      d_ccImag2 -= p_sigIn[6].imag();
      d_ccReal1 += p_sigIn[10].real();
      d_ccImag1 += p_sigIn[10].imag();
      d_ccReal2 -= p_sigIn[14].real();
      d_ccImag2 -= p_sigIn[14].imag();
      d_ccReal1 += p_sigIn[18].real();
      d_ccImag1 += p_sigIn[18].imag();
      d_ccReal2 -= p_sigIn[22].real();
      d_ccImag2 -= p_sigIn[22].imag();
      d_ccReal2 -= p_sigIn[26].real();
      d_ccImag2 -= p_sigIn[26].imag();
      d_ccReal1 += p_sigIn[30].real();
      d_ccImag1 += p_sigIn[30].imag();
      d_ccReal2 -= p_sigIn[34].real();
      d_ccImag2 -= p_sigIn[34].imag();
      d_ccReal2 -= p_sigIn[38].real();
      d_ccImag2 -= p_sigIn[38].imag();
      d_ccReal2 -= p_sigIn[42].real();
      d_ccImag2 -= p_sigIn[42].imag();
      d_ccReal1 += p_sigIn[46].real();
      d_ccImag1 += p_sigIn[46].imag();
      d_ccReal2 -= p_sigIn[50].real();
      d_ccImag2 -= p_sigIn[50].imag();
      d_ccReal1 += p_sigIn[54].real();
      d_ccImag1 += p_sigIn[54].imag();
      d_ccReal1 += p_sigIn[58].real();
      d_ccImag1 += p_sigIn[58].imag();
      d_ccReal1 += p_sigIn[62].real();
      d_ccImag1 += p_sigIn[62].imag();

      d_ccReal1 -= p_sigIn[0].imag();
      d_ccImag1 += p_sigIn[0].real();
      d_ccReal1 += p_sigIn[4].imag();
      d_ccImag1 -= p_sigIn[4].real();
      d_ccReal1 += p_sigIn[8].imag();
      d_ccImag1 -= p_sigIn[8].real();
      d_ccReal1 -= p_sigIn[12].imag();
      d_ccImag1 += p_sigIn[12].real();
      d_ccReal1 += p_sigIn[16].imag();
      d_ccImag1 -= p_sigIn[16].real();
      d_ccReal1 += p_sigIn[20].imag();
      d_ccImag1 -= p_sigIn[20].real();
      d_ccReal1 -= p_sigIn[24].imag();
      d_ccImag1 += p_sigIn[24].real();
      d_ccReal1 -= p_sigIn[28].imag();
      d_ccImag1 += p_sigIn[28].real();
      d_ccReal1 += p_sigIn[32].imag();
      d_ccImag1 -= p_sigIn[32].real();
      d_ccReal1 += p_sigIn[36].imag();
      d_ccImag1 -= p_sigIn[36].real();
      d_ccReal1 += p_sigIn[40].imag();
      d_ccImag1 -= p_sigIn[40].real();
      d_ccReal1 -= p_sigIn[44].imag();
      d_ccImag1 += p_sigIn[44].real();
      d_ccReal1 -= p_sigIn[48].imag();
      d_ccImag1 += p_sigIn[48].real();
      d_ccReal1 -= p_sigIn[52].imag();
      d_ccImag1 += p_sigIn[52].real();
      d_ccReal1 -= p_sigIn[56].imag();
      d_ccImag1 += p_sigIn[56].real();
      d_ccReal1 += p_sigIn[60].imag();
      d_ccImag1 -= p_sigIn[60].real();

      d_ccReal1 = d_ccReal1 + d_ccReal2;
      d_ccImag1 = d_ccImag1 + d_ccImag2;

      return ((d_ccReal1 * d_ccReal1 + d_ccImag1 * d_ccImag1) / p_sigPower);
    }
  } /* namespace advoqpsk */
} /* namespace gr */
