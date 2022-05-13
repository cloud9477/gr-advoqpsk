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

#ifndef INCLUDED_ADVOQPSK_ADVOQPSK_DECODE_IMPL_H
#define INCLUDED_ADVOQPSK_ADVOQPSK_DECODE_IMPL_H

#include <advoqpsk/advoqpsk_decode.h>

namespace gr
{
  namespace advoqpsk
  {
    class advoqpsk_decode_impl : public advoqpsk_decode
    {
    private:
      int d_globalStage; /* find header, get pkt len, get byte*/

      /* constant */
      double *d_sinUnit4mP;
      double *d_sinUnit4mN;
      gr_complexd **d_constByteComp; /*singal complex signal for 255 bytes*/
      gr_complexd **d_constSymbolCompConj;

      /* equalizer */
      gr_complexd *d_linEq;

      /* demodulation */
      gr_complexd *d_demodAdjSigBuf; /* to save the signal without cfo, the demod and equlizer are using it*/
      int d_demodPktLen;             /* current pkt len */
      double d_demodRs;              /* rad step */
      double d_demodRa;              /* rad accum */
      int d_demodByteIdxFromA7;      /* the byte idx, from a7, a7 is 0, pkt len is 1, and 2 and so on */
      int *d_demodBytes;             /* the reasult array*/
      char d_byteBuf[256];

      /* attribute */
      double d_snr;
      double d_cfo;

      /* debug */
      bool d_bDebug;

    public:
      advoqpsk_decode_impl(bool ifDebug);
      ~advoqpsk_decode_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

      void fSigPhaseAdj(const gr_complexd *p_sigInMinus1, gr_complexd *p_sigOutMinus1, double p_phaseDegStep, double p_phaseDegAccum);
      void fLinearEqualizer(gr_complexd *p_sigInMinus1, double p_phaseDegStep, double p_phaseDegAccum, gr_complexd *p_a74mIn, gr_complexd *p_linEqOut);
      void fMatInvLU(gr_complexd mOut[5][5], gr_complexd mIn[5][5], int n);
      void fSigGenRI(int p_byteIn, double *p_sigOutR, double *p_sigOutI, double *p_sinUnitP, double *p_sinUnitN);
      void fSigGenComp(gr_complexd *p_sigOut, double *p_inR, double *p_inI);
      int fGetByte(gr_complexd *p_sigInMinus1, gr_complexd *p_eqIn, double *p_sinUnit);
      double fGetSnrFromZz(const gr_complexd *p_sigIn);
      void fSymbolGenCompConj(int p_symbolIn, gr_complexd *p_sigOutComp, gr_complexd *p_sigByteIn);
      int fGetByte2(gr_complexd *p_sigInMinus1, gr_complexd *p_eqIn);
      double fSymbolCc(gr_complexd *p_sigSymbolIn, gr_complexd *p_sigIn);
    };
  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_ADVOQPSK_DECODE_IMPL_H */
