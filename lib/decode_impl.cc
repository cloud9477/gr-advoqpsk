/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#include <gnuradio/io_signature.h>
#include "decode_impl.h"

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

namespace gr {
  namespace advoqpsk {

    decode::sptr
    decode::make(bool ifDebug)
    {
      return gnuradio::make_block_sptr<decode_impl>(ifDebug
        );
    }


    /*
     * The private constructor
     */
    decode_impl::decode_impl(bool ifDebug)
      : gr::block("decode",
              gr::io_signature::makev(3, 3, std::vector<int>{sizeof(gr_complex), sizeof(float), sizeof(int)}),
              gr::io_signature::make(0, 0, 0)),
              d_bDebug(ifDebug)
    {
      message_port_register_out(pmt::mp("out"));
      set_tag_propagation_policy(block::TPP_DONT);
      d_globalStage = ADVRCVR_STAGE_FINDHEADER;

      /*constant*/
      d_sinUnit4mP = new float[4];
      d_sinUnit4mN = new float[4];
      d_constByteComp = new gr_complex *[256];
      d_constSymbolCompConj = new gr_complex *[16];
      float tmpSigReal[128];
      float tmpSigImag[128];
      for (int i = 0; i < 4; i++)
      {
        d_sinUnit4mP[i] = sin((float)i / 4.0 * M_PI);
        d_sinUnit4mN[i] = -d_sinUnit4mP[i];
      }
      for (int i = 0; i < 256; i++)
      {
        d_constByteComp[i] = new gr_complex[128];
      }
      for (int i = 0; i < 256; i++)
      {
        fSigGenRI(i, tmpSigReal, tmpSigImag, d_sinUnit4mP, d_sinUnit4mN);
        fSigGenComp(d_constByteComp[i], tmpSigReal, tmpSigImag);
      }
      for (int i = 0; i < 16; i++)
      {
        d_constSymbolCompConj[i] = new gr_complex[64];
      }
      for (int i = 0; i < 16; i++)
      {
        fSymbolGenCompConj(i, d_constSymbolCompConj[i], d_constByteComp[i]);
      }

      /* equalizer */
      d_linEq = new gr_complex[5];
      for (int i = 0; i < 5; i++)
      {
        d_linEq[i] = gr_complex(0.0, 0.0);
      }

      /* demodulation */
      d_demodPktLen = 0;
      d_demodRs = 0.0;
      d_demodRa = 0.0;
      d_demodByteIdxFromA7 = 0;
      d_demodBytes = new int[128];
      for (int i = 0; i < 128; i++)
      {
        d_demodBytes[i] = 0;
      }
      d_demodAdjSigBuf = new gr_complex[132];
      for (int i = 0; i < 256; i++)
      {
        d_byteBuf[i] = 0;
      }

      d_pktCount = 0;
    }

    /*
     * Our virtual destructor.
     */
    decode_impl::~decode_impl()
    {
      delete[] d_sinUnit4mP;
      delete[] d_sinUnit4mN;
      delete[] d_linEq;
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

    void
    decode_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items + 512;
      ninput_items_required[1] = noutput_items + 512;
      ninput_items_required[2] = noutput_items + 512;
    }

    int
    decode_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      const gr_complex *inSig = static_cast<const gr_complex*>(input_items[0]);
      const float *inRadStep = static_cast<const float*>(input_items[1]);
      const int *inPre = static_cast<const int*>(input_items[2]);

      int nSampProc = 0;   // number of consumed input samples and generated output samples
      int nInputLimit = ninput_items[0]-512;   // number of limited input samples can be used
      int nSampLimit = 0;
      if(nInputLimit > 0)
      {
        if(noutput_items < nInputLimit){
          nSampLimit = noutput_items;
        }
        else{
          nSampLimit = nInputLimit;
        }
      }

      if(d_globalStage == ADVRCVR_STAGE_FINDHEADER)
      {
        for (int i = 0; i < nSampLimit; i++)
        {
          nSampProc++;
          if (inPre[i])
          {
            d_demodRs = inRadStep[i];
            d_demodRa = 0.0;
            /* use cc of zz3 zz4 to get snr */
            d_snr = fGetSnrFromZz(&inSig[i + 128]);
            d_cfo = d_demodRs * 4000000.0 / 2.0 / M_PI;
            d_globalStage = ADVRCVR_STAGE_GETLINEQ;
            nSampProc--;  //keep the sample pointed which is 1st sample of 2nd zz
            break;
          }
        }
      }
      else if(d_globalStage == ADVRCVR_STAGE_GETLINEQ){
        if(nSampLimit > (512+4)){
          fSigPhaseAdj(&inSig[384 - 1], d_demodAdjSigBuf, d_demodRs, d_demodRa);
          fLinearEqualizer(d_demodAdjSigBuf, d_demodRs, d_demodRa, d_constByteComp[167], d_linEq);
          d_demodRa = d_demodRs * 128.0;
          d_demodByteIdxFromA7 = 1;
          d_globalStage = ADVRCVR_STAGE_GETPKTLEN;
          nSampProc = 512 - 1;
        }
      }
      else if(d_globalStage == ADVRCVR_STAGE_GETPKTLEN){
        if(nSampLimit > (128+4)){
          fSigPhaseAdj(&inSig[0], d_demodAdjSigBuf, d_demodRs, d_demodRa);
          d_demodPktLen = fGetByte2(d_demodAdjSigBuf, d_linEq);
          if (d_demodPktLen < 128)
          {
            fLinearEqualizer(d_demodAdjSigBuf, d_demodRs, d_demodRa, d_constByteComp[d_demodPktLen], d_linEq);
            d_demodBytes[0] = d_demodPktLen;
            d_demodRa += d_demodRs * 128.0;
            d_globalStage = ADVRCVR_STAGE_GETPAYLOAD;
            nSampProc = 128;
          }
          else{
            d_globalStage = ADVRCVR_STAGE_FINDHEADER;
          }
        }
      }
      else if(d_globalStage == ADVRCVR_STAGE_GETPAYLOAD){
        int tmpPtr = 0;
        while((tmpPtr + 132) < nSampLimit){
          if(d_demodByteIdxFromA7 <= d_demodPktLen){
            fSigPhaseAdj(&inSig[tmpPtr], d_demodAdjSigBuf, d_demodRs, d_demodRa);
            int tmpByteDemod = fGetByte2(d_demodAdjSigBuf, d_linEq);
            fLinearEqualizer(d_demodAdjSigBuf, d_demodRs, d_demodRa, d_constByteComp[tmpByteDemod], d_linEq);
            d_demodBytes[d_demodByteIdxFromA7] = tmpByteDemod;
            d_demodByteIdxFromA7++;
            tmpPtr += 128;
            d_demodRa += d_demodRs * 128.0;
            nSampProc += 128;
          }
          else{
            d_pktCount++;
            if (d_bDebug)
            {
              /* print the packet with the CFO and SNR */
              std::cout <<"pktnum:"<<d_pktCount<< "|pktlen:" << d_demodPktLen << "|payload:";
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
            d_globalStage = ADVRCVR_STAGE_FINDHEADER;
            break;
          }
        }
      }
      

      consume_each (nSampProc);
      // Tell runtime system how many output items we produced.
      return nSampProc;
    }

    void
    decode_impl::fSigPhaseAdj(const gr_complex *p_sigInMinus1, gr_complex *p_sigOutMinus1, float p_phaseDegStep, float p_phaseDegAccum)
    {
      for (int i = 0; i < 132; i++)
      {
        p_sigOutMinus1[i] = p_sigInMinus1[i] * gr_complex(cos(i * p_phaseDegStep + p_phaseDegAccum), sin(i * p_phaseDegStep + p_phaseDegAccum));
      }
    }

    void
    decode_impl::fLinearEqualizer(gr_complex *p_sigInMinus1, float p_phaseDegStep, float p_phaseDegAccum, gr_complex *p_a74mIn, gr_complex *p_linEqOut)
    {
      gr_complex tmpMRRH[5][5];
      gr_complex tmpMRRHI[5][5];
      gr_complex tmpMRHRRHI[128][5];
      int i, j, k;
      gr_complex tmpAccum;
      /* get the mat R dot mat RH*/
      for (i = 0; i < 5; i++)
      {
        for (j = 0; j < 5; j++)
        {
          tmpAccum = gr_complex(0.0, 0.0);
          for (k = 0; k < 128; k++)
          {
            tmpAccum += p_sigInMinus1[i + k] * gr_complex(p_sigInMinus1[j + k].real(), -p_sigInMinus1[j + k].imag());
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
          tmpAccum = gr_complex(0.0, 0.0);
          for (k = 0; k < 5; k++)
          {
            tmpAccum += tmpMRRHI[k][i] * gr_complex(p_sigInMinus1[j + k].real(), -p_sigInMinus1[j + k].imag());
          }
          tmpMRHRRHI[j][i] = tmpAccum;
        }
      }
      /* s dot mRHInv(RRH)*/
      for (i = 0; i < 5; i++)
      {
        tmpAccum = gr_complex(0.0, 0.0);
        for (j = 0; j < 128; j++)
        {
          tmpAccum += tmpMRHRRHI[j][i] * p_a74mIn[j];
        }
        p_linEqOut[i] = tmpAccum;
      }
    }

    void
    decode_impl::fMatInvLU(gr_complex mOut[5][5], gr_complex mIn[5][5], int n)
    {
      gr_complex mU[5][5];
      gr_complex mL[5][5];
      gr_complex mUI[5][5];
      gr_complex mLI[5][5];

      int i, j, k, t;
      /* init all mats */
      gr_complex tmpComp0 = gr_complex(0.0, 0.0);
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
          gr_complex s = 0.0;
          for (t = 0; t < k; t++)
            s += mL[k][t] * mU[t][j];
          mU[k][j] = mIn[k][j] - s;
        }
        for (i = k; i < n; i++)
        {
          gr_complex s = 0.0;
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
            mLI[i][j] = gr_complex(1.0, 0.0) / mL[i][j];
          else if (i < j)
            mLI[i][j] = 0;
          else
          {
            gr_complex s = 0.0;
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
            mUI[i][j] = gr_complex(1.0, 0.0) / mU[i][j];
          else if (i > j)
            mUI[i][j] = 0.0;
          else
          {
            gr_complex s = 0.0;
            for (k = i + 1; k <= j; k++)
              s += mU[i][k] * mUI[k][j];
            mUI[i][j] = gr_complex(-1.0, 0.0) / mU[i][i] * s;
          }
        }

      for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
          for (k = 0; k < n; k++)
            mOut[i][j] += mUI[i][k] * mLI[k][j];
    }

    void
    decode_impl::fSigGenRI(int p_byteIn, float *p_sigOutR, float *p_sigOutI, float *p_sinUnitP, float *p_sinUnitN)
    {
      int tmpIdxH, tmpIdxL;
      int tmpBitR[32];
      int tmpBitI[32];
      int i;
      float tmpSigR[130];
      float tmpSigI[130];

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
          memcpy(&tmpSigR[i * 4], p_sinUnitP, 4 * sizeof(float));
        else
          memcpy(&tmpSigR[i * 4], p_sinUnitN, 4 * sizeof(float));
        if (tmpBitI[i] == 1)
          memcpy(&tmpSigI[i * 4 + 2], p_sinUnitP, 4 * sizeof(float));
        else
          memcpy(&tmpSigI[i * 4 + 2], p_sinUnitN, 4 * sizeof(float));
      }
      tmpSigR[128] = 0.0;
      tmpSigR[129] = 0.0;
      tmpSigI[0] = 0.0;
      tmpSigI[1] = 0.0;

      memcpy(p_sigOutR, tmpSigR, 128 * sizeof(float));
      memcpy(p_sigOutI, tmpSigI, 128 * sizeof(float));
    }

    void
    decode_impl::fSigGenComp(gr_complex *p_sigOut, float *p_inR, float *p_inI)
    {
      for (int i = 0; i < 128; i++)
      {
        p_sigOut[i] = gr_complex(p_inR[i], p_inI[i]);
      }
    }

    int
    decode_impl::fGetByte(gr_complex *p_sigInMinus1, gr_complex *p_eqIn, float *p_sinUnit)
    {
      gr_complex tmpSig[128];
      gr_complex tmpConv[131];
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

    float
    decode_impl::fGetSnrFromZz(const gr_complex *p_sigIn)
    {
      gr_complex tmpDotSum = gr_complex(0.0, 0.0);
      float tmpZz3Pwr = 0.0;
      float tmpZz4Pwr = 0.0;
      float tmpRho;
      
      for (int i = 0; i < 128; i++)
      {
        tmpDotSum += p_sigIn[i] * gr_complex(p_sigIn[i + 128].real(), -p_sigIn[i + 128].imag());
        tmpZz3Pwr += p_sigIn[i].real() * p_sigIn[i].real() + p_sigIn[i].imag() * p_sigIn[i].imag();
        tmpZz4Pwr += p_sigIn[i + 128].real() * p_sigIn[i + 128].real() + p_sigIn[i + 128].imag() * p_sigIn[i + 128].imag();
      }
      tmpRho = sqrt((tmpDotSum.real() * tmpDotSum.real() + tmpDotSum.imag() * tmpDotSum.imag()) / tmpZz3Pwr / tmpZz4Pwr);
      return log10(tmpRho / (1 - tmpRho)) * 10.0;
    }

    void
    decode_impl::fSymbolGenCompConj(int p_symbolIn, gr_complex *p_sigOutComp, gr_complex *p_sigByteIn)
    {
      int i;
      for (i = 0; i < 64; i++)
      {
        p_sigOutComp[i] = gr_complex(p_sigByteIn[i].real(), -p_sigByteIn[i].imag());
      }
    }

    int
    decode_impl::fGetByte2(gr_complex *p_sigInMinus1, gr_complex *p_eqIn)
    {
      gr_complex tmpSig[128];
      int tmpByteH, tmpByteL;
      float tmpCc;
      int i;

      for (i = 0; i < 128; i++)
      {
        tmpSig[i] = p_sigInMinus1[i + 0] * p_eqIn[0] + p_sigInMinus1[i + 1] * p_eqIn[1] + p_sigInMinus1[i + 2] * p_eqIn[2] + p_sigInMinus1[i + 3] * p_eqIn[3] + p_sigInMinus1[i + 4] * p_eqIn[4];
      }

      tmpCc = 0.0;
      tmpByteL = 0;
      for (i = 0; i < 16; i++)
      {
        float tmpCcRound = fSymbolCc(&tmpSig[0], d_constSymbolCompConj[i]);
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
        float tmpCcRound = fSymbolCc(&tmpSig[64], d_constSymbolCompConj[i]);
        if (tmpCcRound > tmpCc)
        {
          tmpCc = tmpCcRound;
          tmpByteH = i;
        }
      }

      return tmpByteH * 16 + tmpByteL;
    }

    float
    decode_impl::fSymbolCc(gr_complex *p_sigSymbolIn, gr_complex *p_sigIn)
    {
      int i;
      gr_complex tmpAccum1 = gr_complex(0.0, 0.0);
      gr_complex tmpAccum2 = gr_complex(0.0, 0.0);
      
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
