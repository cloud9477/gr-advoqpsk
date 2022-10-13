/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "sync_impl.h"

namespace gr {
  namespace advoqpsk {

    sync::sptr
    sync::make(float ccThreshold)
    {
      return gnuradio::make_block_sptr<sync_impl>(ccThreshold
        );
    }


    /*
     * The private constructor
     */
    sync_impl::sync_impl(float ccThreshold)
      : gr::block("sync",
              gr::io_signature::makev(6, 6, std::vector<int>{sizeof(gr_complex), sizeof(float), sizeof(int), sizeof(int), sizeof(int), sizeof(int)}),
              gr::io_signature::makev(2, 2, std::vector<int>{sizeof(float), sizeof(int)})),
              d_ccThreshold(ccThreshold)
    {
      d_bufHeadLen = 512; /* 4 bytes, 3 zz (zero zero), 1 a7 */
      d_bufMaxLen = 8192; /* 2ms */
      d_pBufRad = new float[d_bufMaxLen];
      d_pBufCc = new float[d_bufMaxLen];

      for (int i = 0; i < d_bufMaxLen; i++)
      {
        d_pBufRad[i] = 0.0f;
        d_pBufCc[i] = 0.0f;
      }

      d_ccThreshold = d_ccThreshold * d_ccThreshold * 64.0f;
      d_RadStepCompensation = 2.0f * M_PI / 64.0f;
    }

    /*
     * Our virtual destructor.
     */
    sync_impl::~sync_impl()
    {
      delete[] d_pBufRad;
      delete[] d_pBufCc;
      d_pBufRad = NULL;
      d_pBufCc = NULL;
    }

    void
    sync_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items + 512;
      ninput_items_required[1] = noutput_items + 512;
      ninput_items_required[2] = noutput_items + 512;
      ninput_items_required[3] = noutput_items + 512;
      ninput_items_required[4] = noutput_items + 512;
      ninput_items_required[5] = noutput_items + 512;
    }

    int
    sync_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex* inSig = static_cast<const gr_complex*>(input_items[0]);
      const float *inPwr = static_cast<const float*>(input_items[1]);
      const int *inPre0 = static_cast<const int*>(input_items[2]);
      const int *inPre1 = static_cast<const int*>(input_items[3]);
      const int *inPre2 = static_cast<const int*>(input_items[4]);
      const int *inPre3 = static_cast<const int*>(input_items[5]);

      float *outRad = static_cast<float*>(output_items[0]);
      int *outPre = static_cast<int*>(output_items[1]);

      //int nSampProc = 0;   // number of consumed input samples and generated output samples
      int nInputLimit = ninput_items[0]-512;   // number of limited input samples can be used
      int nSampUseLimit = 0;

      if(noutput_items < nInputLimit){
        nSampUseLimit = noutput_items;
      }
      else{
        nSampUseLimit = nInputLimit;
      }
      

      float tmpRad[5], tmpCc, tmpMaxCc;
      int tmpMaxCcIndex;
      for (int i = 0; i < nSampUseLimit; i++)
      {
        if ((inPre0[i] + inPre1[i] + inPre2[i] + inPre3[i]))
        {
          tmpRad[0] = fFindA7CfoRadStep(&inSig[i]);
          tmpRad[1] = tmpRad[0] + d_RadStepCompensation;
          tmpRad[2] = tmpRad[0] - d_RadStepCompensation;
          tmpRad[3] = tmpRad[0] + d_RadStepCompensation * 2.0;
          tmpRad[4] = tmpRad[0] - d_RadStepCompensation * 2.0;
          tmpMaxCcIndex = 0;
          tmpMaxCc = 0.0;
          for (int j = 0; j < 5; j++)
          {
            tmpCc = fCrossCorrelationA7(&inSig[i + 384], inPwr[i + 384] + inPwr[i + 448], tmpRad[j]);
            if (tmpCc > tmpMaxCc)
            {
              tmpMaxCcIndex = j;
              tmpMaxCc = tmpCc;
            }
          }
          d_pBufRad[i] = tmpRad[tmpMaxCcIndex];
          d_pBufCc[i] = tmpMaxCc;
        }
        else
        {
          d_pBufRad[i] = 0.0;
          d_pBufCc[i] = 0.0;
        }
      }

      int tmpFind;
      for (int i = 0; i < nSampUseLimit; i++)
      {
        tmpFind = 0;
        if (d_pBufCc[i] > d_ccThreshold)
        {
          if (i > 0 && i < (nSampUseLimit - 1))
          {
            if (d_pBufCc[i] > d_pBufCc[i - 1] && d_pBufCc[i] > d_pBufCc[i + 1])
            {
              tmpFind = 1;
            }
          }
          else
          {
            tmpFind = 1;
          }
        }
        outPre[i] = 0;
        outRad[i] = 0.0;
        if (tmpFind)
        {
          if (fCrossCorrelationZz(&inSig[i + 256], inPwr[i + 256] + inPwr[i + 320], d_pBufRad[i]) > d_ccThreshold)
          {
            if (fCrossCorrelationZz(&inSig[i + 128], inPwr[i + 128] + inPwr[i + 192], d_pBufRad[i]) > d_ccThreshold)
            {
              if (fCrossCorrelationZz(&inSig[i], inPwr[i] + inPwr[i + 64], d_pBufRad[i]) > d_ccThreshold)
              {
                outPre[i] = 1;
                outRad[i] = d_pBufRad[i];
              }
            }
          }
        }
      }
      
      consume_each (nSampUseLimit);

      // Tell runtime system how many output items we produced.
      return nSampUseLimit;
    }

    float
    sync_impl::fFindA7CfoRadStep(const gr_complex *p_sigIn)
    {
      gr_complex tmpPhaseDiffAccum = gr_complex(0.0, 0.0);
      int i;

      for (i = 0; i < 64; i += 4)
      {
        tmpPhaseDiffAccum += p_sigIn[i] * gr_complex(p_sigIn[i + 64].real(), -p_sigIn[i + 64].imag());
        tmpPhaseDiffAccum += p_sigIn[i + 64] * gr_complex(p_sigIn[i + 128].real(), -p_sigIn[i + 128].imag());
        tmpPhaseDiffAccum += p_sigIn[i + 128] * gr_complex(p_sigIn[i + 192].real(), -p_sigIn[i + 192].imag());
        tmpPhaseDiffAccum += p_sigIn[i + 192] * gr_complex(p_sigIn[i + 256].real(), -p_sigIn[i + 256].imag());
      }
      tmpPhaseDiffAccum = tmpPhaseDiffAccum / 64.0f;
      return (atan2(tmpPhaseDiffAccum.imag(), tmpPhaseDiffAccum.real()) / 64.0);
    }

    float
    sync_impl::fCrossCorrelationA7(const gr_complex *p_sigIn, float p_sigPwrIn, float p_phaseDegStep)
    {
      gr_complex tmpAdjSigIn[128];
      float d_ccReal1 = 0.0f;
      float d_ccReal2 = 0.0f;
      float d_ccImag1 = 0.0f;
      float d_ccImag2 = 0.0f;
      for (int i = 0; i < 128; i += 2)
      {
        tmpAdjSigIn[i] = p_sigIn[i] * gr_complex(cos(i * p_phaseDegStep), sin(i * p_phaseDegStep));
      }

      d_ccReal1 += tmpAdjSigIn[2].real();
      d_ccImag1 += tmpAdjSigIn[2].imag();
      d_ccReal2 -= tmpAdjSigIn[6].real();
      d_ccImag2 -= tmpAdjSigIn[6].imag();
      d_ccReal1 += tmpAdjSigIn[10].real();
      d_ccImag1 += tmpAdjSigIn[10].imag();
      d_ccReal2 -= tmpAdjSigIn[14].real();
      d_ccImag2 -= tmpAdjSigIn[14].imag();
      d_ccReal1 -= tmpAdjSigIn[18].real();
      d_ccImag1 -= tmpAdjSigIn[18].imag();
      d_ccReal2 += tmpAdjSigIn[22].real();
      d_ccImag2 += tmpAdjSigIn[22].imag();
      d_ccReal1 -= tmpAdjSigIn[26].real();
      d_ccImag1 -= tmpAdjSigIn[26].imag();
      d_ccReal2 -= tmpAdjSigIn[30].real();
      d_ccImag2 -= tmpAdjSigIn[30].imag();
      d_ccReal1 -= tmpAdjSigIn[34].real();
      d_ccImag1 -= tmpAdjSigIn[34].imag();
      d_ccReal2 += tmpAdjSigIn[38].real();
      d_ccImag2 += tmpAdjSigIn[38].imag();
      d_ccReal1 -= tmpAdjSigIn[42].real();
      d_ccImag1 -= tmpAdjSigIn[42].imag();
      d_ccReal2 += tmpAdjSigIn[46].real();
      d_ccImag2 += tmpAdjSigIn[46].imag();
      d_ccReal1 += tmpAdjSigIn[50].real();
      d_ccImag1 += tmpAdjSigIn[50].imag();
      d_ccReal2 += tmpAdjSigIn[54].real();
      d_ccImag2 += tmpAdjSigIn[54].imag();
      d_ccReal1 += tmpAdjSigIn[58].real();
      d_ccImag1 += tmpAdjSigIn[58].imag();
      d_ccReal2 -= tmpAdjSigIn[62].real();
      d_ccImag2 -= tmpAdjSigIn[62].imag();
      d_ccReal1 -= tmpAdjSigIn[66].real();
      d_ccImag1 -= tmpAdjSigIn[66].imag();
      d_ccReal2 += tmpAdjSigIn[70].real();
      d_ccImag2 += tmpAdjSigIn[70].imag();
      d_ccReal1 += tmpAdjSigIn[74].real();
      d_ccImag1 += tmpAdjSigIn[74].imag();
      d_ccReal2 += tmpAdjSigIn[78].real();
      d_ccImag2 += tmpAdjSigIn[78].imag();
      d_ccReal1 += tmpAdjSigIn[82].real();
      d_ccImag1 += tmpAdjSigIn[82].imag();
      d_ccReal2 -= tmpAdjSigIn[86].real();
      d_ccImag2 -= tmpAdjSigIn[86].imag();
      d_ccReal1 += tmpAdjSigIn[90].real();
      d_ccImag1 += tmpAdjSigIn[90].imag();
      d_ccReal2 -= tmpAdjSigIn[94].real();
      d_ccImag2 -= tmpAdjSigIn[94].imag();
      d_ccReal1 += tmpAdjSigIn[98].real();
      d_ccImag1 += tmpAdjSigIn[98].imag();
      d_ccReal2 -= tmpAdjSigIn[102].real();
      d_ccImag2 -= tmpAdjSigIn[102].imag();
      d_ccReal1 -= tmpAdjSigIn[106].real();
      d_ccImag1 -= tmpAdjSigIn[106].imag();
      d_ccReal2 += tmpAdjSigIn[110].real();
      d_ccImag2 += tmpAdjSigIn[110].imag();
      d_ccReal1 -= tmpAdjSigIn[114].real();
      d_ccImag1 -= tmpAdjSigIn[114].imag();
      d_ccReal2 -= tmpAdjSigIn[118].real();
      d_ccImag2 -= tmpAdjSigIn[118].imag();
      d_ccReal1 -= tmpAdjSigIn[122].real();
      d_ccImag1 -= tmpAdjSigIn[122].imag();
      d_ccReal2 += tmpAdjSigIn[126].real();
      d_ccImag2 += tmpAdjSigIn[126].imag();

      d_ccReal1 -= tmpAdjSigIn[0].imag();
      d_ccImag1 += tmpAdjSigIn[0].real();
      d_ccReal2 -= tmpAdjSigIn[4].imag();
      d_ccImag2 += tmpAdjSigIn[4].real();
      d_ccReal1 += tmpAdjSigIn[8].imag();
      d_ccImag1 -= tmpAdjSigIn[8].real();
      d_ccReal2 += tmpAdjSigIn[12].imag();
      d_ccImag2 -= tmpAdjSigIn[12].real();
      d_ccReal1 -= tmpAdjSigIn[16].imag();
      d_ccImag1 += tmpAdjSigIn[16].real();
      d_ccReal2 -= tmpAdjSigIn[20].imag();
      d_ccImag2 += tmpAdjSigIn[20].real();
      d_ccReal1 += tmpAdjSigIn[24].imag();
      d_ccImag1 -= tmpAdjSigIn[24].real();
      d_ccReal2 += tmpAdjSigIn[28].imag();
      d_ccImag2 -= tmpAdjSigIn[28].real();
      d_ccReal1 += tmpAdjSigIn[32].imag();
      d_ccImag1 -= tmpAdjSigIn[32].real();
      d_ccReal2 -= tmpAdjSigIn[36].imag();
      d_ccImag2 += tmpAdjSigIn[36].real();
      d_ccReal1 -= tmpAdjSigIn[40].imag();
      d_ccImag1 += tmpAdjSigIn[40].real();
      d_ccReal2 -= tmpAdjSigIn[44].imag();
      d_ccImag2 += tmpAdjSigIn[44].real();
      d_ccReal1 -= tmpAdjSigIn[48].imag();
      d_ccImag1 += tmpAdjSigIn[48].real();
      d_ccReal2 += tmpAdjSigIn[52].imag();
      d_ccImag2 -= tmpAdjSigIn[52].real();
      d_ccReal1 -= tmpAdjSigIn[56].imag();
      d_ccImag1 += tmpAdjSigIn[56].real();
      d_ccReal2 += tmpAdjSigIn[60].imag();
      d_ccImag2 -= tmpAdjSigIn[60].real();
      d_ccReal1 += tmpAdjSigIn[64].imag();
      d_ccImag1 -= tmpAdjSigIn[64].real();
      d_ccReal2 += tmpAdjSigIn[68].imag();
      d_ccImag2 -= tmpAdjSigIn[68].real();
      d_ccReal1 += tmpAdjSigIn[72].imag();
      d_ccImag1 -= tmpAdjSigIn[72].real();
      d_ccReal2 -= tmpAdjSigIn[76].imag();
      d_ccImag2 += tmpAdjSigIn[76].real();
      d_ccReal1 += tmpAdjSigIn[80].imag();
      d_ccImag1 -= tmpAdjSigIn[80].real();
      d_ccReal2 -= tmpAdjSigIn[84].imag();
      d_ccImag2 += tmpAdjSigIn[84].real();
      d_ccReal1 -= tmpAdjSigIn[88].imag();
      d_ccImag1 += tmpAdjSigIn[88].real();
      d_ccReal2 += tmpAdjSigIn[92].imag();
      d_ccImag2 -= tmpAdjSigIn[92].real();
      d_ccReal1 -= tmpAdjSigIn[96].imag();
      d_ccImag1 += tmpAdjSigIn[96].real();
      d_ccReal2 -= tmpAdjSigIn[100].imag();
      d_ccImag2 += tmpAdjSigIn[100].real();
      d_ccReal1 += tmpAdjSigIn[104].imag();
      d_ccImag1 -= tmpAdjSigIn[104].real();
      d_ccReal2 += tmpAdjSigIn[108].imag();
      d_ccImag2 -= tmpAdjSigIn[108].real();
      d_ccReal1 -= tmpAdjSigIn[112].imag();
      d_ccImag1 += tmpAdjSigIn[112].real();
      d_ccReal2 -= tmpAdjSigIn[116].imag();
      d_ccImag2 += tmpAdjSigIn[116].real();
      d_ccReal1 -= tmpAdjSigIn[120].imag();
      d_ccImag1 += tmpAdjSigIn[120].real();
      d_ccReal2 += tmpAdjSigIn[124].imag();
      d_ccImag2 -= tmpAdjSigIn[124].real();

      d_ccReal1 = d_ccReal1 + d_ccReal2;
      d_ccImag1 = d_ccImag1 + d_ccImag2;

      return ((d_ccReal1 * d_ccReal1 + d_ccImag1 * d_ccImag1) / p_sigPwrIn);
    }

    float
    sync_impl::fCrossCorrelationZz(const gr_complex *p_sigIn, float p_sigPwrIn, float p_phaseDegStep)
    {
      gr_complex tmpAdjSigIn[128];
      float d_ccReal1 = 0.0;
      float d_ccReal2 = 0.0;
      float d_ccImag1 = 0.0;
      float d_ccImag2 = 0.0;
      for (int i = 0; i < 128; i += 2)
      {
        tmpAdjSigIn[i] = p_sigIn[i] * gr_complex(cos(i * p_phaseDegStep), sin(i * p_phaseDegStep));
      }

      d_ccReal1 += tmpAdjSigIn[2].real();
      d_ccImag1 += tmpAdjSigIn[2].imag();
      d_ccReal2 -= tmpAdjSigIn[6].real();
      d_ccImag2 -= tmpAdjSigIn[6].imag();
      d_ccReal1 += tmpAdjSigIn[10].real();
      d_ccImag1 += tmpAdjSigIn[10].imag();
      d_ccReal2 -= tmpAdjSigIn[14].real();
      d_ccImag2 -= tmpAdjSigIn[14].imag();
      d_ccReal1 += tmpAdjSigIn[18].real();
      d_ccImag1 += tmpAdjSigIn[18].imag();
      d_ccReal2 -= tmpAdjSigIn[22].real();
      d_ccImag2 -= tmpAdjSigIn[22].imag();
      d_ccReal1 -= tmpAdjSigIn[26].real();
      d_ccImag1 -= tmpAdjSigIn[26].imag();
      d_ccReal2 += tmpAdjSigIn[30].real();
      d_ccImag2 += tmpAdjSigIn[30].imag();
      d_ccReal1 -= tmpAdjSigIn[34].real();
      d_ccImag1 -= tmpAdjSigIn[34].imag();
      d_ccReal2 -= tmpAdjSigIn[38].real();
      d_ccImag2 -= tmpAdjSigIn[38].imag();
      d_ccReal1 -= tmpAdjSigIn[42].real();
      d_ccImag1 -= tmpAdjSigIn[42].imag();
      d_ccReal2 += tmpAdjSigIn[46].real();
      d_ccImag2 += tmpAdjSigIn[46].imag();
      d_ccReal1 -= tmpAdjSigIn[50].real();
      d_ccImag1 -= tmpAdjSigIn[50].imag();
      d_ccReal2 += tmpAdjSigIn[54].real();
      d_ccImag2 += tmpAdjSigIn[54].imag();
      d_ccReal1 += tmpAdjSigIn[58].real();
      d_ccImag1 += tmpAdjSigIn[58].imag();
      d_ccReal2 += tmpAdjSigIn[62].real();
      d_ccImag2 += tmpAdjSigIn[62].imag();
      d_ccReal1 += tmpAdjSigIn[66].real();
      d_ccImag1 += tmpAdjSigIn[66].imag();
      d_ccReal2 -= tmpAdjSigIn[70].real();
      d_ccImag2 -= tmpAdjSigIn[70].imag();
      d_ccReal1 += tmpAdjSigIn[74].real();
      d_ccImag1 += tmpAdjSigIn[74].imag();
      d_ccReal2 -= tmpAdjSigIn[78].real();
      d_ccImag2 -= tmpAdjSigIn[78].imag();
      d_ccReal1 += tmpAdjSigIn[82].real();
      d_ccImag1 += tmpAdjSigIn[82].imag();
      d_ccReal2 -= tmpAdjSigIn[86].real();
      d_ccImag2 -= tmpAdjSigIn[86].imag();
      d_ccReal1 -= tmpAdjSigIn[90].real();
      d_ccImag1 -= tmpAdjSigIn[90].imag();
      d_ccReal2 += tmpAdjSigIn[94].real();
      d_ccImag2 += tmpAdjSigIn[94].imag();
      d_ccReal1 -= tmpAdjSigIn[98].real();
      d_ccImag1 -= tmpAdjSigIn[98].imag();
      d_ccReal2 -= tmpAdjSigIn[102].real();
      d_ccImag2 -= tmpAdjSigIn[102].imag();
      d_ccReal1 -= tmpAdjSigIn[106].real();
      d_ccImag1 -= tmpAdjSigIn[106].imag();
      d_ccReal2 += tmpAdjSigIn[110].real();
      d_ccImag2 += tmpAdjSigIn[110].imag();
      d_ccReal1 -= tmpAdjSigIn[114].real();
      d_ccImag1 -= tmpAdjSigIn[114].imag();
      d_ccReal2 += tmpAdjSigIn[118].real();
      d_ccImag2 += tmpAdjSigIn[118].imag();
      d_ccReal1 += tmpAdjSigIn[122].real();
      d_ccImag1 += tmpAdjSigIn[122].imag();
      d_ccReal2 += tmpAdjSigIn[126].real();
      d_ccImag2 += tmpAdjSigIn[126].imag();

      d_ccReal1 -= tmpAdjSigIn[0].imag();
      d_ccImag1 += tmpAdjSigIn[0].real();
      d_ccReal2 += tmpAdjSigIn[4].imag();
      d_ccImag2 -= tmpAdjSigIn[4].real();
      d_ccReal1 += tmpAdjSigIn[8].imag();
      d_ccImag1 -= tmpAdjSigIn[8].real();
      d_ccReal2 -= tmpAdjSigIn[12].imag();
      d_ccImag2 += tmpAdjSigIn[12].real();
      d_ccReal1 += tmpAdjSigIn[16].imag();
      d_ccImag1 -= tmpAdjSigIn[16].real();
      d_ccReal2 += tmpAdjSigIn[20].imag();
      d_ccImag2 -= tmpAdjSigIn[20].real();
      d_ccReal1 -= tmpAdjSigIn[24].imag();
      d_ccImag1 += tmpAdjSigIn[24].real();
      d_ccReal2 -= tmpAdjSigIn[28].imag();
      d_ccImag2 += tmpAdjSigIn[28].real();
      d_ccReal1 += tmpAdjSigIn[32].imag();
      d_ccImag1 -= tmpAdjSigIn[32].real();
      d_ccReal2 += tmpAdjSigIn[36].imag();
      d_ccImag2 -= tmpAdjSigIn[36].real();
      d_ccReal1 += tmpAdjSigIn[40].imag();
      d_ccImag1 -= tmpAdjSigIn[40].real();
      d_ccReal2 -= tmpAdjSigIn[44].imag();
      d_ccImag2 += tmpAdjSigIn[44].real();
      d_ccReal1 -= tmpAdjSigIn[48].imag();
      d_ccImag1 += tmpAdjSigIn[48].real();
      d_ccReal2 -= tmpAdjSigIn[52].imag();
      d_ccImag2 += tmpAdjSigIn[52].real();
      d_ccReal1 -= tmpAdjSigIn[56].imag();
      d_ccImag1 += tmpAdjSigIn[56].real();
      d_ccReal2 += tmpAdjSigIn[60].imag();
      d_ccImag2 -= tmpAdjSigIn[60].real();
      d_ccReal1 -= tmpAdjSigIn[64].imag();
      d_ccImag1 += tmpAdjSigIn[64].real();
      d_ccReal2 += tmpAdjSigIn[68].imag();
      d_ccImag2 -= tmpAdjSigIn[68].real();
      d_ccReal1 += tmpAdjSigIn[72].imag();
      d_ccImag1 -= tmpAdjSigIn[72].real();
      d_ccReal2 -= tmpAdjSigIn[76].imag();
      d_ccImag2 += tmpAdjSigIn[76].real();
      d_ccReal1 += tmpAdjSigIn[80].imag();
      d_ccImag1 -= tmpAdjSigIn[80].real();
      d_ccReal2 += tmpAdjSigIn[84].imag();
      d_ccImag2 -= tmpAdjSigIn[84].real();
      d_ccReal1 -= tmpAdjSigIn[88].imag();
      d_ccImag1 += tmpAdjSigIn[88].real();
      d_ccReal2 -= tmpAdjSigIn[92].imag();
      d_ccImag2 += tmpAdjSigIn[92].real();
      d_ccReal1 += tmpAdjSigIn[96].imag();
      d_ccImag1 -= tmpAdjSigIn[96].real();
      d_ccReal2 += tmpAdjSigIn[100].imag();
      d_ccImag2 -= tmpAdjSigIn[100].real();
      d_ccReal1 += tmpAdjSigIn[104].imag();
      d_ccImag1 -= tmpAdjSigIn[104].real();
      d_ccReal2 -= tmpAdjSigIn[108].imag();
      d_ccImag2 += tmpAdjSigIn[108].real();
      d_ccReal1 -= tmpAdjSigIn[112].imag();
      d_ccImag1 += tmpAdjSigIn[112].real();
      d_ccReal2 -= tmpAdjSigIn[116].imag();
      d_ccImag2 += tmpAdjSigIn[116].real();
      d_ccReal1 -= tmpAdjSigIn[120].imag();
      d_ccImag1 += tmpAdjSigIn[120].real();
      d_ccReal2 += tmpAdjSigIn[124].imag();
      d_ccImag2 -= tmpAdjSigIn[124].real();

      d_ccReal1 = d_ccReal1 + d_ccReal2;
      d_ccImag1 = d_ccImag1 + d_ccImag2;

      return ((d_ccReal1 * d_ccReal1 + d_ccImag1 * d_ccImag1) / p_sigPwrIn);
    }

  } /* namespace advoqpsk */
} /* namespace gr */
