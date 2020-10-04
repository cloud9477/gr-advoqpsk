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
#include "advoqpsk_decode_impl.h"

#define ADVRCVR_STAGE_FINDHEADER 1
#define ADVRCVR_STAGE_GETLINEQ 2
#define ADVRCVR_STAGE_GETPKTLEN 3
#define ADVRCVR_STAGE_GETPAYLOAD 4

static const int ADVRCVR_CONST_CHIPLIST[16][32] =
    {{1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0},
     {1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0},
     {0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0},
     {0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1},
     {0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1},
     {0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0},
     {1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 1},
     {1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 1, 0, 1},
     {1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1},
     {1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1},
     {0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1},
     {0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0},
     {0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0},
     {0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 1},
     {1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 0, 0},
     {1, 1, 0, 0, 1, 0, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0}};

namespace gr
{
  namespace advoqpsk
  {
    advoqpsk_decode::sptr
    advoqpsk_decode::make(bool ifDebug)
    {
      return gnuradio::get_initial_sptr(new advoqpsk_decode_impl(ifDebug));
    }

    /*
     * The private constructor
     */
    static int ios[] = {sizeof(gr_complexd), sizeof(double), sizeof(int)};
    static std::vector<int> iosig(ios, ios + sizeof(ios) / sizeof(int));
    advoqpsk_decode_impl::advoqpsk_decode_impl(bool ifDebug)
        : gr::sync_block("advoqpsk_decode",
                         gr::io_signature::makev(3, 3, iosig),
                         gr::io_signature::make(0, 0, 0)),
          d_bDebug(ifDebug)
    {
      message_port_register_out(pmt::mp("out"));

      d_bufHeadLen = 512; /* zz2 zz3 zz4 a7 */
      d_bufMaxLen = 8192; /* 2ms */
      d_bufCurrLen = d_bufHeadLen;
      d_pBuf = new gr_complexd[d_bufMaxLen];
      d_pBufRad = new double[d_bufMaxLen];
      d_pBufPre = new int[d_bufMaxLen];
      for (int i = 0; i < d_bufHeadLen; i++)
      {
        d_pBuf[i] = gr_complexd(1.0, 1.0);
        d_pBufRad[i] = 0.0;
        d_pBufPre[i] = 0;
      }
      d_globalStage = ADVRCVR_STAGE_FINDHEADER;

      /*constant*/
      d_sinUnit4mP = new double[4];
      d_sinUnit4mN = new double[4];
      d_constByteComp = new gr_complexd *[256];
      d_constSymbolCompConj = new gr_complexd *[16];
      double tmpSigReal[128];
      double tmpSigImag[128];
      for (int i = 0; i < 4; i++)
      {
        d_sinUnit4mP[i] = sin((double)i / 4.0 * M_PI);
        d_sinUnit4mN[i] = -d_sinUnit4mP[i];
      }
      for (int i = 0; i < 256; i++)
      {
        d_constByteComp[i] = new gr_complexd[128];
      }
      for (int i = 0; i < 256; i++)
      {
        fSigGenRI(i, tmpSigReal, tmpSigImag, d_sinUnit4mP, d_sinUnit4mN);
        fSigGenComp(d_constByteComp[i], tmpSigReal, tmpSigImag);
      }
      for (int i = 0; i < 16; i++)
      {
        d_constSymbolCompConj[i] = new gr_complexd[64];
      }
      for (int i = 0; i < 16; i++)
      {
        fSymbolGenCompConj(i, d_constSymbolCompConj[i], d_constByteComp[i]);
      }

      /* equalizer */
      d_linEq = new gr_complexd[5];
      for (int i = 0; i < 5; i++)
      {
        d_linEq[i] = gr_complexd(0.0, 0.0);
      }

      /* demodulation */
      d_demodPtr = d_bufHeadLen - 128;
      d_demodPktLen = 0;
      d_demodRs = 0.0;
      d_demodRa = 0.0;
      d_demodByteIdxFromA7 = 0;
      d_demodPtrRemain = 0;
      d_demodBytes = new int[128];
      for (int i = 0; i < 128; i++)
      {
        d_demodBytes[i] = 0;
      }
      d_demodAdjSigBuf = new gr_complexd[132];
      for (int i = 0; i < 256; i++)
      {
        d_byteBuf[i] = 0;
      }
    }

    /*
     * Our virtual destructor.
     */
    advoqpsk_decode_impl::~advoqpsk_decode_impl()
    {
      delete[] d_pBuf;
      delete[] d_sinUnit4mP;
      delete[] d_sinUnit4mN;
      delete[] d_linEq;
      d_pBuf = NULL;
      d_sinUnit4mP = NULL;
      d_sinUnit4mN = NULL;
      d_linEq = NULL;
      for (int i = 0; i < 256; i++)
      {
        delete[] d_constByteComp[i];
      }
      delete[] d_constByteComp;
      d_constByteComp = NULL;
      for (int i = 0; i < 16; i++)
      {
        delete[] d_constSymbolCompConj[i];
      }
      delete[] d_constSymbolCompConj;
      d_constSymbolCompConj = NULL;
    }

    int
    advoqpsk_decode_impl::work(int noutput_items,
                               gr_vector_const_void_star &input_items,
                               gr_vector_void_star &output_items)
    {
      const gr_complexd *inSig = (const gr_complexd *)input_items[0];
      double *inRadStep = (double *)input_items[1];
      int *inPre = (int *)input_items[2];

      memcpy(&d_pBuf[d_bufHeadLen], inSig, noutput_items * sizeof(gr_complexd));
      memcpy(&d_pBufRad[d_bufHeadLen], inRadStep, noutput_items * sizeof(double));
      memcpy(&d_pBufPre[d_bufHeadLen], inPre, noutput_items * sizeof(int));
      d_bufCurrLen = d_bufHeadLen + noutput_items;
      d_bKeepLoop = 1;

      while (d_bKeepLoop)
      {
        if (d_globalStage == ADVRCVR_STAGE_FINDHEADER)
        {
          int tmpFindFlag = 0;
          for (int i = d_demodPtrRemain; i < noutput_items; i++)
          {
            if (d_pBufPre[i + d_bufHeadLen])
            {
              d_demodRs = d_pBufRad[i + d_bufHeadLen];
              d_demodRa = 0.0;
              d_demodPtr = i + d_bufHeadLen - 128;
              /* use cc of zz3 zz4 to get snr */
              d_snr = fGetSnrFromZz(&d_pBuf[i + (d_bufHeadLen - 128) - 256]);
              d_cfo = d_demodRs * 4000000.0 / 2.0 / M_PI;
              d_globalStage = ADVRCVR_STAGE_GETLINEQ;
              tmpFindFlag = 1;
              //tmpSampCounter = d_sampCounter + (i+(d_bufHeadLen-128)-512-512);
              break;
            }
          }
          if (!tmpFindFlag)
          {
            /*stop the loop and get to next round*/
            d_demodPtrRemain = 0;
            d_bKeepLoop = 0;
          }
        }

        if (d_globalStage == ADVRCVR_STAGE_GETLINEQ)
        {
          if ((d_demodPtr + 131) <= d_bufCurrLen)
          {
            fSigPhaseAdj(&d_pBuf[d_demodPtr - 1], d_demodAdjSigBuf, d_demodRs, d_demodRa);
            fLinearEqualizer(d_demodAdjSigBuf, d_demodRs, d_demodRa, d_constByteComp[167], d_linEq);
            d_demodPtr += 128;
            d_demodRa = d_demodRs * 128.0;
            d_demodByteIdxFromA7 = 1;
            d_globalStage = ADVRCVR_STAGE_GETPKTLEN;
          }
          else
          {
            /*move the ptr and get to next round*/
            d_demodPtr -= noutput_items;
            d_bKeepLoop = 0;
          }
        }

        if (d_globalStage == ADVRCVR_STAGE_GETPKTLEN)
        {
          if ((d_demodPtr + 131) <= d_bufCurrLen)
          {
            /* get a byte */
            fSigPhaseAdj(&d_pBuf[d_demodPtr - 1], d_demodAdjSigBuf, d_demodRs, d_demodRa);
            d_demodPktLen = fGetByte2(d_demodAdjSigBuf, d_linEq);
            if (d_demodPktLen < 128)
            {
              fLinearEqualizer(d_demodAdjSigBuf, d_demodRs, d_demodRa, d_constByteComp[d_demodPktLen], d_linEq);
              d_demodBytes[0] = d_demodPktLen;
              d_demodPtr += 128;
              d_demodRa += d_demodRs * 128.0;
              d_globalStage = ADVRCVR_STAGE_GETPAYLOAD;
              //std::cout<<"find valid header and sample #:"<<tmpSampCounter<<std::endl;
            }
            else
            {
              /* packet length wrong, back to find header*/
              d_globalStage = ADVRCVR_STAGE_FINDHEADER;
              d_demodPtrRemain = d_demodPtr + 1 - d_bufHeadLen;
            }
          }
          else
          {
            /*move the ptr, data not enough, go to next round*/
            d_demodPtr -= noutput_items;
            d_bKeepLoop = 0;
          }
        }

        if (d_globalStage == ADVRCVR_STAGE_GETPAYLOAD)
        {
          int tmpFinishFlag = 0;
          while (1)
          {
            if (d_demodByteIdxFromA7 <= d_demodPktLen)
            {
              if ((d_demodPtr + 131) <= d_bufCurrLen)
              {
                fSigPhaseAdj(&d_pBuf[d_demodPtr - 1], d_demodAdjSigBuf, d_demodRs, d_demodRa);
                int tmpByteDemod = fGetByte2(d_demodAdjSigBuf, d_linEq);
                fLinearEqualizer(d_demodAdjSigBuf, d_demodRs, d_demodRa, d_constByteComp[tmpByteDemod], d_linEq);
                d_demodBytes[d_demodByteIdxFromA7] = tmpByteDemod;
                d_demodByteIdxFromA7++;
                d_demodPtr += 128;
                d_demodRa += d_demodRs * 128.0;
              }
              else
              {
                /*move the ptr*/
                d_demodPtr -= noutput_items;
                break;
              }
            }
            else
            {
              /* get the whole packet */
              /* reset the parameters */
              tmpFinishFlag = 1;
              if (d_bDebug)
              {
                /* print the packet with the CFO and SNR */
                std::cout << "pktlen:" << d_demodPktLen << "|payload:";
                for (int j = 1; j < (d_demodPktLen + 1); j++)
                {
                  std::cout << d_demodBytes[j] << " ";
                }
                std::cout << "|cfo:" << (d_cfo) << "|snr:" << d_snr << std::endl;
              }
              /* to be compatible to WIME project, send it to MAC */
              unsigned char lqi = 255;
              pmt::pmt_t meta = pmt::make_dict();
              meta = pmt::dict_add(meta, pmt::mp("lqi"), pmt::from_long(lqi));
              unsigned char tmpOutputByteBuf[128];
              for (int j = 1; j < (d_demodPktLen + 1); j++)
              {
                tmpOutputByteBuf[j - 1] = d_demodBytes[j];
              }
              memcpy(d_byteBuf, tmpOutputByteBuf, d_demodPktLen);
              pmt::pmt_t payload = pmt::make_blob(d_byteBuf, d_demodPktLen);
              message_port_pub(pmt::mp("out"), pmt::cons(meta, payload));
              break;
            }
          }

          if (tmpFinishFlag)
          {
            /*back to the finding header*/
            d_globalStage = ADVRCVR_STAGE_FINDHEADER;
            d_demodPtrRemain = d_demodPtr + 1 - d_bufHeadLen;
          }
          else
          {
            /*not finish, data not enough, go to next round*/
            d_bKeepLoop = 0;
          }
        }
      }

      memcpy(&d_pBuf[0], &d_pBuf[noutput_items], d_bufHeadLen * sizeof(gr_complexd));
      memcpy(&d_pBufRad[0], &d_pBufRad[noutput_items], d_bufHeadLen * sizeof(double));
      memcpy(&d_pBufPre[0], &d_pBufPre[noutput_items], d_bufHeadLen * sizeof(int));

      return noutput_items;
    }

    void
    advoqpsk_decode_impl::fSigPhaseAdj(gr_complexd *p_sigInMinus1, gr_complexd *p_sigOutMinus1, double p_phaseDegStep, double p_phaseDegAccum)
    {
      for (int i = 0; i < 132; i++)
      {
        p_sigOutMinus1[i] = p_sigInMinus1[i] * gr_complexd(cos(i * p_phaseDegStep + p_phaseDegAccum), sin(i * p_phaseDegStep + p_phaseDegAccum));
      }
    }

    void
    advoqpsk_decode_impl::fLinearEqualizer(gr_complexd *p_sigInMinus1, double p_phaseDegStep, double p_phaseDegAccum, gr_complexd *p_a74mIn, gr_complexd *p_linEqOut)
    {
      gr_complexd tmpMRRH[5][5];
      gr_complexd tmpMRRHI[5][5];
      gr_complexd tmpMRHRRHI[128][5];
      int i, j, k;
      gr_complexd tmpAccum;
      /* get the mat R dot mat RH*/
      for (i = 0; i < 5; i++)
      {
        for (j = 0; j < 5; j++)
        {
          tmpAccum = gr_complexd(0.0, 0.0);
          for (k = 0; k < 128; k++)
          {
            tmpAccum += p_sigInMinus1[i + k] * gr_complexd(p_sigInMinus1[j + k].real(), -p_sigInMinus1[j + k].imag());
          }
          tmpMRRH[i][j] = tmpAccum;
        }
      }
      /* get the reverse */
      fMatInvLU(tmpMRRHI, tmpMRRH, 5);
      /* RH dot Inv(RRH)*/
      for (i = 0; i < 5; i++)
      {
        for (j = 0; j < 128; j++)
        {
          tmpAccum = gr_complexd(0.0, 0.0);
          for (k = 0; k < 5; k++)
          {
            tmpAccum += tmpMRRHI[k][i] * gr_complexd(p_sigInMinus1[j + k].real(), -p_sigInMinus1[j + k].imag());
          }
          tmpMRHRRHI[j][i] = tmpAccum;
        }
      }
      /* s dot mRHInv(RRH)*/
      for (i = 0; i < 5; i++)
      {
        tmpAccum = gr_complexd(0.0, 0.0);
        for (j = 0; j < 128; j++)
        {
          tmpAccum += tmpMRHRRHI[j][i] * p_a74mIn[j];
        }
        p_linEqOut[i] = tmpAccum;
      }
    }

    void
    advoqpsk_decode_impl::fMatInvLU(gr_complexd mOut[5][5], gr_complexd mIn[5][5], int n)
    {
      gr_complexd mU[5][5];
      gr_complexd mL[5][5];
      gr_complexd mUI[5][5];
      gr_complexd mLI[5][5];

      int i, j, k, t;
      /* init all mats */
      gr_complexd tmpComp0 = gr_complex(0.0, 0.0);
      for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
        {
          mU[i][j] = tmpComp0;
          mL[i][j] = tmpComp0;
          mUI[i][j] = tmpComp0;
          mLI[i][j] = tmpComp0;
          mOut[i][j] = tmpComp0;
        }

      /* fiil 1 in the L diagonal */
      for (i = 0; i < n; i++)
        mL[i][i] = 1.0;
      /* get U first row */
      for (j = 0; j < n; j++)
        mU[0][j] = mIn[0][j];
      /* get L first col */
      for (i = 1; i < n; i++)
        mL[i][0] = mIn[i][0] / mU[0][0];
      /* get the others of the L and U */
      for (k = 1; k < n; k++)
      {
        for (j = k; j < n; j++)
        {
          gr_complexd s = 0.0;
          for (t = 0; t < k; t++)
            s += mL[k][t] * mU[t][j];
          mU[k][j] = mIn[k][j] - s;
        }
        for (i = k; i < n; i++)
        {
          gr_complexd s = 0.0;
          for (t = 0; t < k; t++)
            s += mL[i][t] * mU[t][k];
          mL[i][k] = (mIn[i][k] - s) / mU[k][k];
        }
      }
      /* get the L inverse */
      for (j = 0; j < n; j++)
        for (i = j; i < n; i++)
        {
          if (i == j)
            mLI[i][j] = gr_complexd(1.0, 0.0) / mL[i][j];
          else if (i < j)
            mLI[i][j] = 0;
          else
          {
            gr_complexd s = 0.0;
            for (k = j; k < i; k++)
              s += mL[i][k] * mLI[k][j];
            mLI[i][j] = -mLI[j][j] * s;
          }
        }
      /* get the U inverse */
      for (j = 0; j < n; j++)
        for (i = j; i >= 0; i--)
        {
          if (i == j)
            mUI[i][j] = gr_complexd(1.0, 0.0) / mU[i][j];
          else if (i > j)
            mUI[i][j] = 0.0;
          else
          {
            gr_complexd s = 0.0;
            for (k = i + 1; k <= j; k++)
              s += mU[i][k] * mUI[k][j];
            mUI[i][j] = gr_complexd(-1.0, 0.0) / mU[i][i] * s;
          }
        }

      for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
          for (k = 0; k < n; k++)
            mOut[i][j] += mUI[i][k] * mLI[k][j];
    }

    void
    advoqpsk_decode_impl::fSigGenRI(int p_byteIn, double *p_sigOutR, double *p_sigOutI, double *p_sinUnitP, double *p_sinUnitN)
    {
      int tmpIdxH, tmpIdxL;
      int tmpBitR[32];
      int tmpBitI[32];
      int i;
      double tmpSigR[130];
      double tmpSigI[130];

      tmpIdxH = p_byteIn / 16;
      tmpIdxL = p_byteIn % 16;

      for (i = 0; i < 16; i++)
      {
        tmpBitR[i] = ADVRCVR_CONST_CHIPLIST[tmpIdxL][i * 2];
        tmpBitI[i] = ADVRCVR_CONST_CHIPLIST[tmpIdxL][i * 2 + 1];
      }
      for (i = 0; i < 16; i++)
      {
        tmpBitR[i + 16] = ADVRCVR_CONST_CHIPLIST[tmpIdxH][i * 2];
        tmpBitI[i + 16] = ADVRCVR_CONST_CHIPLIST[tmpIdxH][i * 2 + 1];
      }
      for (i = 0; i < 32; i++)
      {
        if (tmpBitR[i] == 1)
          memcpy(&tmpSigR[i * 4], p_sinUnitP, 4 * sizeof(double));
        else
          memcpy(&tmpSigR[i * 4], p_sinUnitN, 4 * sizeof(double));
        if (tmpBitI[i] == 1)
          memcpy(&tmpSigI[i * 4 + 2], p_sinUnitP, 4 * sizeof(double));
        else
          memcpy(&tmpSigI[i * 4 + 2], p_sinUnitN, 4 * sizeof(double));
      }
      tmpSigR[128] = 0.0;
      tmpSigR[129] = 0.0;
      tmpSigI[0] = 0.0;
      tmpSigI[1] = 0.0;

      memcpy(p_sigOutR, tmpSigR, 128 * sizeof(double));
      memcpy(p_sigOutI, tmpSigI, 128 * sizeof(double));
    }

    void
    advoqpsk_decode_impl::fSigGenComp(gr_complexd *p_sigOut, double *p_inR, double *p_inI)
    {
      for (int i = 0; i < 128; i++)
      {
        p_sigOut[i] = gr_complexd(p_inR[i], p_inI[i]);
      }
    }

    int
    advoqpsk_decode_impl::fGetByte(gr_complexd *p_sigInMinus1, gr_complexd *p_eqIn, double *p_sinUnit)
    {
      gr_complexd tmpSig[128];
      gr_complexd tmpConv[131];
      int i, j;
      int tmpByteH, tmpByteL;
      int tmpScore;
      int tmpBit[64];

      for (i = 0; i < 128; i++)
      {
        tmpSig[i] = p_sigInMinus1[i + 0] * p_eqIn[0] + p_sigInMinus1[i + 1] * p_eqIn[1] + p_sigInMinus1[i + 2] * p_eqIn[2] + p_sigInMinus1[i + 3] * p_eqIn[3] + p_sigInMinus1[i + 4] * p_eqIn[4];
      }
      for (i = 0; i < 131; i++)
      {
        tmpConv[i] = gr_complex(0.0f, 0.0f);
      }
      for (i = 0; i < 128; i++)
      {
        for (j = 0; j < 4; j++)
        {
          tmpConv[i + j] += tmpSig[i] * p_sinUnit[3 - j]; //flip lr
        }
      }
      for (i = 0; i < 32; i++)
      {
        if (tmpConv[i * 4 + 3].real() > 0)
        {
          tmpBit[i * 2] = 1;
        }
        else
        {
          tmpBit[i * 2] = 0;
        }
        if (tmpConv[i * 4 + 5].imag() > 0)
        {
          tmpBit[i * 2 + 1] = 1;
        }
        else
        {
          tmpBit[i * 2 + 1] = 0;
        }
      }

      tmpByteH = 0;
      tmpByteL = 0;
      tmpScore = 0;
      for (i = 0; i < 16; i++)
      {
        int tmpCurrentScore = 0;
        for (j = 0; j < 32; j++)
        {
          if (tmpBit[j] == ADVRCVR_CONST_CHIPLIST[i][j])
          {
            tmpCurrentScore++;
          }
        }
        if (tmpCurrentScore > tmpScore)
        {
          tmpByteL = i;
          tmpScore = tmpCurrentScore;
        }
      }

      tmpScore = 0;
      for (i = 0; i < 16; i++)
      {
        int tmpCurrentScore = 0;
        for (j = 0; j < 32; j++)
        {
          if (tmpBit[j + 32] == ADVRCVR_CONST_CHIPLIST[i][j])
          {
            tmpCurrentScore++;
          }
        }
        if (tmpCurrentScore > tmpScore)
        {
          tmpByteH = i;
          tmpScore = tmpCurrentScore;
        }
      }
      return tmpByteH * 16 + tmpByteL;
    }

    double
    advoqpsk_decode_impl::fGetSnrFromZz(gr_complexd *p_sigIn)
    {
      gr_complexd tmpDotSum = gr_complexd(0.0, 0.0);
      double tmpZz3Pwr = 0.0;
      double tmpZz4Pwr = 0.0;
      double tmpRho;
      
      for (int i = 0; i < 128; i++)
      {
        tmpDotSum += p_sigIn[i] * gr_complexd(p_sigIn[i + 128].real(), -p_sigIn[i + 128].imag());
        tmpZz3Pwr += p_sigIn[i].real() * p_sigIn[i].real() + p_sigIn[i].imag() * p_sigIn[i].imag();
        tmpZz4Pwr += p_sigIn[i + 128].real() * p_sigIn[i + 128].real() + p_sigIn[i + 128].imag() * p_sigIn[i + 128].imag();
      }
      tmpRho = sqrt((tmpDotSum.real() * tmpDotSum.real() + tmpDotSum.imag() * tmpDotSum.imag()) / tmpZz3Pwr / tmpZz4Pwr);
      return log10(tmpRho / (1 - tmpRho)) * 10.0;
    }

    void
    advoqpsk_decode_impl::fSymbolGenCompConj(int p_symbolIn, gr_complexd *p_sigOutComp, gr_complexd *p_sigByteIn)
    {
      int i;
      for (i = 0; i < 64; i++)
      {
        p_sigOutComp[i] = gr_complexd(p_sigByteIn[i].real(), -p_sigByteIn[i].imag());
      }
    }

    int
    advoqpsk_decode_impl::fGetByte2(gr_complexd *p_sigInMinus1, gr_complexd *p_eqIn)
    {
      gr_complexd tmpSig[128];
      int tmpByteH, tmpByteL;
      double tmpCc;
      int i, j;

      for (i = 0; i < 128; i++)
      {
        tmpSig[i] = p_sigInMinus1[i + 0] * p_eqIn[0] + p_sigInMinus1[i + 1] * p_eqIn[1] + p_sigInMinus1[i + 2] * p_eqIn[2] + p_sigInMinus1[i + 3] * p_eqIn[3] + p_sigInMinus1[i + 4] * p_eqIn[4];
      }

      tmpCc = 0.0;
      tmpByteL = 0;
      for (i = 0; i < 16; i++)
      {
        double tmpCcRound = fSymbolCc(&tmpSig[0], d_constSymbolCompConj[i]);
        if (tmpCcRound > tmpCc)
        {
          tmpCc = tmpCcRound;
          tmpByteL = i;
        }
      }

      tmpCc = 0.0;
      tmpByteH = 0;
      for (i = 0; i < 16; i++)
      {
        double tmpCcRound = fSymbolCc(&tmpSig[64], d_constSymbolCompConj[i]);
        if (tmpCcRound > tmpCc)
        {
          tmpCc = tmpCcRound;
          tmpByteH = i;
        }
      }

      return tmpByteH * 16 + tmpByteL;
    }

    double
    advoqpsk_decode_impl::fSymbolCc(gr_complexd *p_sigSymbolIn, gr_complexd *p_sigIn)
    {
      int i;
      gr_complexd tmpAccum1 = gr_complexd(0.0, 0.0);
      gr_complexd tmpAccum2 = gr_complexd(0.0, 0.0);
      
      for (i = 0; i < 32; i++)
      {
        tmpAccum1 += p_sigSymbolIn[i * 2] * p_sigIn[i * 2];
        tmpAccum2 += p_sigSymbolIn[i * 2 + 1] * p_sigIn[i * 2 + 1];
      }
      tmpAccum1 = tmpAccum1 + tmpAccum2;
      return (tmpAccum1.real() * tmpAccum1.real() + tmpAccum1.imag() * tmpAccum1.imag());
    }

  } /* namespace advoqpsk */
} /* namespace gr */
