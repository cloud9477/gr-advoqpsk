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
#include "advoqpsk_sync_impl.h"

namespace gr
{
  namespace advoqpsk
  {
    advoqpsk_sync::sptr
    advoqpsk_sync::make(float ccThreshold)
    {
      return gnuradio::get_initial_sptr(new advoqpsk_sync_impl(ccThreshold));
    }

    /*
     * The private constructor
     */
    /* sig, power, pre1, pre2, pre3 */
    static int iosIn[] = {sizeof(gr_complexd), sizeof(double), sizeof(int), sizeof(int), sizeof(int), sizeof(int)};
    static std::vector<int> iosigIn(iosIn, iosIn + sizeof(iosIn) / sizeof(int));
    /* pre, radStep */
    static int iosOut[] = {sizeof(double), sizeof(int)};
    static std::vector<int> iosigOut(iosOut, iosOut + sizeof(iosOut) / sizeof(int));
    advoqpsk_sync_impl::advoqpsk_sync_impl(float ccThreshold)
        : gr::block("advoqpsk_sync",
                         gr::io_signature::makev(6, 6, iosigIn),
                         gr::io_signature::makev(2, 2, iosigOut)),
          d_ccThreshold(ccThreshold)
    {
      d_bufHeadLen = 512; /* 4 bytes, 3 zz (zero zero), 1 a7 */
      d_bufMaxLen = 8192; /* 2ms */
      d_pBufRad = new double[d_bufMaxLen];
      d_pBufCc = new double[d_bufMaxLen];

      for (int i = 0; i < d_bufMaxLen; i++)
      {
        d_pBufRad[i] = 0.0;
        d_pBufCc[i] = 0.0;
      }

      d_ccThreshold = d_ccThreshold * d_ccThreshold * 64;
      d_RadStepCompensation = 2.0 * M_PI / 64.0;
    }

    /*
     * Our virtual destructor.
     */
    advoqpsk_sync_impl::~advoqpsk_sync_impl()
    {
      delete[] d_pBufRad;
      delete[] d_pBufCc;
      d_pBufRad = NULL;
      d_pBufCc = NULL;
    }

    void
    advoqpsk_sync_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      gr_vector_int::size_type ninputs = ninput_items_required.size();
      for(int i=0; i < ninputs; i++)
      {
	      ninput_items_required[i] = noutput_items + 512;
      }
    }

    int
    advoqpsk_sync_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complexd *inSig = (const gr_complexd *)input_items[0];
      const double *inPwr = (const double *)input_items[1];
      const int *inPre0 = (const int *)input_items[2];
      const int *inPre1 = (const int *)input_items[3];
      const int *inPre2 = (const int *)input_items[4];
      const int *inPre3 = (const int *)input_items[5];

      double *outRad = (double *)output_items[0];
      int *outPre = (int *)output_items[1];

      int nSampProc = 0;   // number of consumed input samples and generated output samples
      int nInputLimit = ninput_items[0]-512;   // number of limited input samples can be used
      int nSampUseLimit = 0;

      if(noutput_items < nInputLimit){
        nSampUseLimit = noutput_items;
      }
      else{
        nSampUseLimit = nInputLimit;
      }
      

      double tmpRad[5], tmpCc, tmpMaxCc;
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

    double
    advoqpsk_sync_impl::fFindA7CfoRadStep(const gr_complexd *p_sigIn)
    {
      gr_complexd tmpPhaseDiffAccum = gr_complexd(0.0, 0.0);
      int i;

      for (i = 0; i < 64; i += 4)
      {
        tmpPhaseDiffAccum += p_sigIn[i] * gr_complexd(p_sigIn[i + 64].real(), -p_sigIn[i + 64].imag());
        tmpPhaseDiffAccum += p_sigIn[i + 64] * gr_complexd(p_sigIn[i + 128].real(), -p_sigIn[i + 128].imag());
        tmpPhaseDiffAccum += p_sigIn[i + 128] * gr_complexd(p_sigIn[i + 192].real(), -p_sigIn[i + 192].imag());
        tmpPhaseDiffAccum += p_sigIn[i + 192] * gr_complexd(p_sigIn[i + 256].real(), -p_sigIn[i + 256].imag());
      }
      tmpPhaseDiffAccum = tmpPhaseDiffAccum / 64.0;
      return (atan2(tmpPhaseDiffAccum.imag(), tmpPhaseDiffAccum.real()) / 64.0);
    }

    double
    advoqpsk_sync_impl::fCrossCorrelationA7(const gr_complexd *p_sigIn, double p_sigPwrIn, double p_phaseDegStep)
    {
      gr_complexd tmpAdjSigIn[128];
      double d_ccReal1 = 0.0;
      double d_ccReal2 = 0.0;
      double d_ccImag1 = 0.0;
      double d_ccImag2 = 0.0;
      for (int i = 0; i < 128; i += 2)
      {
        tmpAdjSigIn[i] = p_sigIn[i] * gr_complexd(cos(i * p_phaseDegStep), sin(i * p_phaseDegStep));
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

    double
    advoqpsk_sync_impl::fCrossCorrelationZz(const gr_complexd *p_sigIn, double p_sigPwrIn, double p_phaseDegStep)
    {
      gr_complexd tmpAdjSigIn[128];
      double d_ccReal1 = 0.0;
      double d_ccReal2 = 0.0;
      double d_ccImag1 = 0.0;
      double d_ccImag2 = 0.0;
      for (int i = 0; i < 128; i += 2)
      {
        tmpAdjSigIn[i] = p_sigIn[i] * gr_complexd(cos(i * p_phaseDegStep), sin(i * p_phaseDegStep));
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
