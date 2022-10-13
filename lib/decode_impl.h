/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ADVOQPSK_DECODE_IMPL_H
#define INCLUDED_ADVOQPSK_DECODE_IMPL_H

#include <gnuradio/advoqpsk/decode.h>

namespace gr {
  namespace advoqpsk {

    class decode_impl : public decode
    {
     private:
      int d_globalStage; /* find header, get pkt len, get byte*/

      /* constant */
      float *d_sinUnit4mP;
      float *d_sinUnit4mN;
      gr_complex **d_constByteComp; /*singal complex signal for 255 bytes*/
      gr_complex **d_constSymbolCompConj;

      /* equalizer */
      gr_complex *d_linEq;

      /* demodulation */
      gr_complex *d_demodAdjSigBuf; /* to save the signal without cfo, the demod and equlizer are using it*/
      int d_demodPktLen;             /* current pkt len */
      float d_demodRs;              /* rad step */
      float d_demodRa;              /* rad accum */
      int d_demodByteIdxFromA7;      /* the byte idx, from a7, a7 is 0, pkt len is 1, and 2 and so on */
      int *d_demodBytes;             /* the reasult array*/
      char d_byteBuf[256];

      /* attribute */
      float d_snr;
      float d_cfo;

      /* debug */
      bool d_bDebug;
      uint64_t d_pktCount;

     public:
      decode_impl(bool ifDebug);
      ~decode_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
      
      void fSigPhaseAdj(const gr_complex *p_sigInMinus1, gr_complex *p_sigOutMinus1, float p_phaseDegStep, float p_phaseDegAccum);
      void fLinearEqualizer(gr_complex *p_sigInMinus1, float p_phaseDegStep, float p_phaseDegAccum, gr_complex *p_a74mIn, gr_complex *p_linEqOut);
      void fMatInvLU(gr_complex mOut[5][5], gr_complex mIn[5][5], int n);
      void fSigGenRI(int p_byteIn, float *p_sigOutR, float *p_sigOutI, float *p_sinUnitP, float *p_sinUnitN);
      void fSigGenComp(gr_complex *p_sigOut, float *p_inR, float *p_inI);
      int fGetByte(gr_complex *p_sigInMinus1, gr_complex *p_eqIn, float *p_sinUnit);
      float fGetSnrFromZz(const gr_complex *p_sigIn);
      void fSymbolGenCompConj(int p_symbolIn, gr_complex *p_sigOutComp, gr_complex *p_sigByteIn);
      int fGetByte2(gr_complex *p_sigInMinus1, gr_complex *p_eqIn);
      float fSymbolCc(gr_complex *p_sigSymbolIn, gr_complex *p_sigIn);

    };

  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_DECODE_IMPL_H */
