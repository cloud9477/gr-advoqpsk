/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ADVOQPSK_SYNC_H
#define INCLUDED_ADVOQPSK_SYNC_H

#include <gnuradio/advoqpsk/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace advoqpsk {

    /*!
     * \brief <+description of block+>
     * \ingroup advoqpsk
     *
     */
    class ADVOQPSK_API sync : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<sync> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of advoqpsk::sync.
       *
       * To avoid accidental use of raw pointers, advoqpsk::sync's
       * constructor is in a private implementation
       * class. advoqpsk::sync::make is the public interface for
       * creating new instances.
       */
      static sptr make(float ccThreshold);
    };

  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_SYNC_H */
