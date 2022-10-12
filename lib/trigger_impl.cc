/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "trigger_impl.h"

namespace gr {
  namespace advoqpsk {

    trigger::sptr
    trigger::make(float ccThreshold)
    {
      return gnuradio::make_block_sptr<trigger_impl>(ccThreshold
        );
    }


    /*
     * The private constructor
     */
    trigger_impl::trigger_impl(float ccThreshold)
      : gr::block("trigger",
              gr::io_signature::makev(2, 2, std::vector<int>{sizeof(gr_complex), sizeof(float)}),
              gr::io_signature::make(1, 1, sizeof(uint8_t))),
              d_ccThreshold(ccThreshold)
    {
      d_bufHeadLen = 512; /* 4 bytes */
      d_bufMaxLen = 8192;           /* 2ms */
      d_pBufCc7 = new int[d_bufMaxLen];
      d_pBufCcZ = new int[d_bufMaxLen];
      for (int i = 0; i < d_bufHeadLen; i++)
      {
        d_pBufCc7[i] = 0;
        d_pBufCcZ[i] = 0;
      }
      d_ccThreshold = d_ccThreshold * d_ccThreshold * 32.0;
    }

    /*
     * Our virtual destructor.
     */
    trigger_impl::~trigger_impl()
    {
      delete[] d_pBufCc7;
      delete[] d_pBufCcZ;
      d_pBufCc7 = NULL;
      d_pBufCcZ = NULL;
    }

    void
    trigger_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items + d_bufHeadLen;
      ninput_items_required[1] = noutput_items + d_bufHeadLen;
    }

    int
    trigger_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex* inSig = static_cast<const gr_complex*>(input_items[0]);
      const float* inPwr = static_cast<const float*>(input_items[0]);

      uint8_t* out = static_cast<uint8_t*>(output_items[0]);

      int nSampProc = 0;   // number of consumed input samples and generated output samples
      int nInputLimit = ninput_items[0]-d_bufHeadLen;   // number of limited input samples can be used

      if(nInputLimit > 0)
      {
        while(nSampProc < noutput_items && nSampProc < nInputLimit)
        {
          if (fCcZ(&inSig[nSampProc + 384], inPwr[nSampProc + 384]) > d_ccThreshold)
          {
            d_pBufCcZ[nSampProc + 384] = 1;
          }
          else
          {
            d_pBufCcZ[nSampProc + 384] = 0;
          }
          if (fCc7(&inSig[nSampProc + 384], inPwr[nSampProc + 384]) > d_ccThreshold)
          {
            d_pBufCc7[nSampProc + 384] = 1;
          }
          else
          {
            d_pBufCc7[nSampProc + 384] = 0;
          }
          out[nSampProc] = 0;
          if (fCcA(&inSig[nSampProc + 448], inPwr[nSampProc + 448]) > d_ccThreshold)
          {
            if (d_pBufCc7[nSampProc + 384] && d_pBufCcZ[nSampProc + 320] && d_pBufCcZ[nSampProc + 256] && d_pBufCcZ[nSampProc + 192] && d_pBufCcZ[nSampProc + 128] && d_pBufCcZ[nSampProc + 64] && d_pBufCcZ[nSampProc])
            {
              out[nSampProc] = 1;
            }
          }
          nSampProc++;
        }
      }

      memcpy(&d_pBufCc7[0], &d_pBufCc7[nSampProc], d_bufHeadLen * sizeof(int));
      memcpy(&d_pBufCcZ[0], &d_pBufCcZ[nSampProc], d_bufHeadLen * sizeof(int));

      // Tell runtime system how many input smaples consumed.
      consume_each (nSampProc);
      // Tell runtime system how many output items we produced.
      return nSampProc;
    }

    float
    trigger_impl::fCc7(const gr_complex *p_sigIn, float p_sigPower)
    {
      float d_ccReal1 = 0.0f;
      float d_ccReal2 = 0.0f;
      float d_ccImag1 = 0.0f;
      float d_ccImag2 = 0.0f;

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

    float
    trigger_impl::fCcA(const gr_complex *p_sigIn, float p_sigPower)
    {
      float d_ccReal1 = 0.0f;
      float d_ccReal2 = 0.0f;
      float d_ccImag1 = 0.0f;
      float d_ccImag2 = 0.0f;

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

    float
    trigger_impl::fCcZ(const gr_complex *p_sigIn, float p_sigPower)
    {
      float d_ccReal1 = 0.0f;
      float d_ccReal2 = 0.0f;
      float d_ccImag1 = 0.0f;
      float d_ccImag2 = 0.0f;

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
