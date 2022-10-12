/* -*- c++ -*- */
/*
 * Copyright 2022 Zelin Yun.
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */

#ifndef INCLUDED_ADVOQPSK_PRECFOFIX_H
#define INCLUDED_ADVOQPSK_PRECFOFIX_H

#include <gnuradio/advoqpsk/api.h>
#include <gnuradio/sync_block.h>

namespace gr {
  namespace advoqpsk {

    /*!
     * \brief <+description of block+>
     * \ingroup advoqpsk
     *
     */
    class ADVOQPSK_API precfofix : virtual public gr::sync_block
    {
     public:
      typedef std::shared_ptr<precfofix> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of advoqpsk::precfofix.
       *
       * To avoid accidental use of raw pointers, advoqpsk::precfofix's
       * constructor is in a private implementation
       * class. advoqpsk::precfofix::make is the public interface for
       * creating new instances.
       */
      static sptr make(int cfoin);
    };

  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_PRECFOFIX_H */
