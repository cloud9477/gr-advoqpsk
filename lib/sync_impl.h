/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ADVOQPSK_SYNC_IMPL_H
#define INCLUDED_ADVOQPSK_SYNC_IMPL_H

#include <gnuradio/advoqpsk/sync.h>

namespace gr {
  namespace advoqpsk {

    class sync_impl : public sync
    {
     private:
      int d_bufHeadLen;
      int d_bufMaxLen;
      float *d_pBufRad;
      float *d_pBufCc;
      float d_ccThreshold;
      float d_RadStepCompensation;

     public:
      sync_impl(float ccThreshold);
      ~sync_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
      float fFindA7CfoRadStep(const gr_complex *p_sigIn);
      float fCrossCorrelationA7(const gr_complex *p_sigIn, float p_sigPwrIn, float p_phaseDegStep);
      float fCrossCorrelationZz(const gr_complex *p_sigIn, float p_sigPwrIn, float p_phaseDegStep);
    };

  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_SYNC_IMPL_H */
