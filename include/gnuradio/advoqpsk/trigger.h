/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ADVOQPSK_TRIGGER_H
#define INCLUDED_ADVOQPSK_TRIGGER_H

#include <gnuradio/advoqpsk/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace advoqpsk {

    /*!
     * \brief <+description of block+>
     * \ingroup advoqpsk
     *
     */
    class ADVOQPSK_API trigger : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<trigger> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of advoqpsk::trigger.
       *
       * To avoid accidental use of raw pointers, advoqpsk::trigger's
       * constructor is in a private implementation
       * class. advoqpsk::trigger::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_TRIGGER_H */
