#include <gnuradio/io_signature.h>
#include "prefixer_impl.h"

namespace gr {
  namespace advoqpsk {

    prefixer::sptr
    prefixer::make()
    {
      return gnuradio::make_block_sptr<prefixer_impl>(
        );
    }


    /*
     * The private constructor
     */
    prefixer_impl::prefixer_impl()
      : gr::block("prefixer",
              gr::io_signature::make(0, 0, 0),
              gr::io_signature::make(0, 0, 0))
    {
      message_port_register_out(pmt::mp("out"));
      message_port_register_in(pmt::mp("in"));
      set_msg_handler(pmt::mp("in"), boost::bind(&prefixer_impl::make_frame, this, boost::placeholders::_1));
      d_buf[0] = 0;     // pad
      d_buf[1] = 0;  // preamble
      d_buf[2] = 0;
      d_buf[3] = 0;
      d_buf[4] = 0xa7;
    }

    /*
     * Our virtual destructor.
     */
    prefixer_impl::~prefixer_impl()
    {
    }

    void prefixer_impl::make_frame(pmt::pmt_t msg)
    {
        assert(pmt::is_pair(msg));
        pmt::pmt_t blob = pmt::cdr(msg);

        size_t data_len = pmt::blob_length(blob);
        assert(data_len);
        assert(data_len < 256 - 5);

        d_buf[5] = data_len;

        std::memcpy(d_buf + 6, pmt::blob_data(blob), data_len);

        for(int i=0;i<(int)data_len+5;i++)
        {
          std::cout<<(int)d_buf[i]<<" ";
        }
        std::cout<<std::endl;

        pmt::pmt_t packet = pmt::make_blob(d_buf, data_len + 6);

        message_port_pub(pmt::mp("out"), pmt::cons(pmt::PMT_NIL, packet));
    }

    void
    prefixer_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
    }

    int
    prefixer_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      consume_each (0);
      return 0;
    }

  } /* namespace advoqpsk */
} /* namespace gr */
