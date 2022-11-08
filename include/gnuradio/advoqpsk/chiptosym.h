#ifndef INCLUDED_ADVOQPSK_CHIPTOSYM_H
#define INCLUDED_ADVOQPSK_CHIPTOSYM_H

#include <gnuradio/advoqpsk/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace advoqpsk {

    /*!
     * \brief <+description of block+>
     * \ingroup advoqpsk
     *
     */
    class ADVOQPSK_API chiptosym : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<chiptosym> sptr;

      /*!
       * \brief Return a shared_ptr to a new instance of advoqpsk::chiptosym.
       *
       * To avoid accidental use of raw pointers, advoqpsk::chiptosym's
       * constructor is in a private implementation
       * class. advoqpsk::chiptosym::make is the public interface for
       * creating new instances.
       */
      static sptr make();
    };

  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_CHIPTOSYM_H */
