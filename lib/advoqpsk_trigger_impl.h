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

#ifndef INCLUDED_ADVOQPSK_ADVOQPSK_TRIGGER_IMPL_H
#define INCLUDED_ADVOQPSK_ADVOQPSK_TRIGGER_IMPL_H

#include <advoqpsk/advoqpsk_trigger.h>

namespace gr
{
  namespace advoqpsk
  {
    class advoqpsk_trigger_impl : public advoqpsk_trigger
    {
    private:
      int d_bufHeadLen;
      int d_bufMaxLen;
      int *d_pBufCc7;
      int *d_pBufCcZ;
      double d_ccThreshold;

    public:
      advoqpsk_trigger_impl(float ccThreshold);
      ~advoqpsk_trigger_impl();

      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);
      
      double fCcA(const gr_complexd *p_sigIn, double p_sigPower);     /* the cross correlation of symbol a*/
      double fCc7(const gr_complexd *p_sigIn, double p_sigPower);     /* the cross correlation of symbol 7*/
      double fCcZ(const gr_complexd *p_sigIn, double p_sigPower);     /* the cross correlation of symbol 0 (z for zero)*/
    };
  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_ADVOQPSK_TRIGGER_IMPL_H */
