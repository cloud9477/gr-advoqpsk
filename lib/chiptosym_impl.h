

#ifndef INCLUDED_ADVOQPSK_CHIPTOSYM_IMPL_H
#define INCLUDED_ADVOQPSK_CHIPTOSYM_IMPL_H

#include <gnuradio/advoqpsk/chiptosym.h>

namespace gr {
  namespace advoqpsk {

    class chiptosym_impl : public chiptosym
    {
     private:

     public:
      chiptosym_impl();
      ~chiptosym_impl();

      // Where all the action really happens
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

    };

  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_CHIPTOSYM_IMPL_H */
