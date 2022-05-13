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


#ifndef INCLUDED_ADVOQPSK_ADVOQPSK_POWERSTEP_H
#define INCLUDED_ADVOQPSK_ADVOQPSK_POWERSTEP_H

#include <advoqpsk/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace advoqpsk {

    /*!
     * \brief <+description of block+>
     * \ingroup advoqpsk
     *
     */
    class ADVOQPSK_API advoqpsk_powerstep : virtual public gr::block
    {
     public:
      typedef boost::shared_ptr<advoqpsk_powerstep> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of advoqpsk::advoqpsk_powerstep.
       *
       * To avoid accidental use of raw pointers, advoqpsk::advoqpsk_powerstep's
       * constructor is in a private implementation
       * class. advoqpsk::advoqpsk_powerstep::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_ADVOQPSK_POWERSTEP_H */

