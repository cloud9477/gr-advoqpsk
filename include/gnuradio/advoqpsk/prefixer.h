#ifndef INCLUDED_ADVOQPSK_PREFIXER_H
#define INCLUDED_ADVOQPSK_PREFIXER_H

#include <gnuradio/advoqpsk/api.h>
#include <gnuradio/block.h>

namespace gr {
  namespace advoqpsk {

    /*!
     * \brief <+description of block+>
     * \ingroup advoqpsk
     *
     */
    class ADVOQPSK_API prefixer : virtual public gr::block
    {
     public:
      typedef std::shared_ptr<prefixer> sptr;
      static sptr make();
    };

  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_PREFIXER_H */
