#ifndef INCLUDED_ADVOQPSK_PREFIXER_IMPL_H
#define INCLUDED_ADVOQPSK_PREFIXER_IMPL_H

#include <gnuradio/advoqpsk/prefixer.h>
#include <boost/bind/bind.hpp>
#include <gnuradio/pdu.h>


namespace gr {
  namespace advoqpsk {

    class prefixer_impl : public prefixer
    {
     private:
      uint8_t d_buf[256];

     public:
      prefixer_impl();
      ~prefixer_impl();

      void make_frame(pmt::pmt_t msg);
      void forecast (int noutput_items, gr_vector_int &ninput_items_required);

      int general_work(int noutput_items,
           gr_vector_int &ninput_items,
           gr_vector_const_void_star &input_items,
           gr_vector_void_star &output_items);

    };

  } // namespace advoqpsk
} // namespace gr

#endif /* INCLUDED_ADVOQPSK_PREFIXER_IMPL_H */
