

#include <gnuradio/io_signature.h>
#include "chiptosym_impl.h"

namespace gr {
  namespace advoqpsk {

    chiptosym::sptr
    chiptosym::make()
    {
      return gnuradio::make_block_sptr<chiptosym_impl>(
        );
    }


    /*
     * The private constructor
     */
    chiptosym_impl::chiptosym_impl()
      : gr::block("chiptosym",
              gr::io_signature::make(1, 1, sizeof(uint8_t)),
              gr::io_signature::make(1, 1, sizeof(gr_complex)))
    {
      // set_tag_propagation_policy(block::TPP_ALL_TO_ALL);
    }

    /*
     * Our virtual destructor.
     */
    chiptosym_impl::~chiptosym_impl()
    {
    }

    void
    chiptosym_impl::forecast (int noutput_items, gr_vector_int &ninput_items_required)
    {
      ninput_items_required[0] = noutput_items;
    }

    int
    chiptosym_impl::general_work (int noutput_items,
                       gr_vector_int &ninput_items,
                       gr_vector_const_void_star &input_items,
                       gr_vector_void_star &output_items)
    {
      static const gr_complex lut[256] = {
        gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(1.0f, 1.0f), gr_complex(-1.0f,-1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f, 1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f), gr_complex(-1.0f,-1.0f), 
        gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), gr_complex(-1.0f, 1.0f), gr_complex(1.0f, 1.0f), 
        gr_complex(1.0f,-1.0f), gr_complex(1.0f, 1.0f), gr_complex(1.0f,-1.0f), gr_complex(-1.0f,-1.0f)};

      const uint8_t* inChip = static_cast<const uint8_t*>(input_items[0]);
      gr_complex* outSym = static_cast<gr_complex*>(output_items[0]);

      //std::cout<<"cloud gr-adv: chip to sym work function start"<<std::endl;

      std::vector<gr::tag_t> tags;
      get_tags_in_range(tags, 0, nitems_read(0) , nitems_read(0) + 1);
      if(tags.size())
      {
        std::cout<<"cloud gr-advoqpsk: 4-bit chip to symbol, get the tag"<<std::endl;
        pmt::pmt_t d_meta = pmt::make_dict();
        for (auto tag : tags){
          d_meta = pmt::dict_add(d_meta, tag.key, tag.value);
        }
        int t_len = pmt::to_long(pmt::dict_ref(d_meta, pmt::mp("packet_len"), pmt::from_long(-1)));
        std::cout<<"cloud gr-advoqpsk: 4-bit chip to symbol, packet_len: "<<t_len<<std::endl;
        pmt::pmt_t dict = pmt::make_dict();
        dict = pmt::dict_add(dict, pmt::mp("packet_len"), pmt::from_long(t_len));
        pmt::pmt_t pairs = pmt::dict_items(dict);
        for (size_t i = 0; i < pmt::length(pairs); i++) {
            pmt::pmt_t pair = pmt::nth(i, pairs);
            add_item_tag(0,                   // output port index
                          nitems_written(0),  // output sample index
                          pmt::car(pair),
                          pmt::cdr(pair),
                          alias_pmt());
        }
      }

      int symlen = 16;

      for(int i=0; i<noutput_items;i++)
      {
        if(inChip[i] >= 0 && inChip[i] < symlen)
        {
          memcpy((uint8_t*)&outSym[i*symlen], (uint8_t*)&lut[inChip[i]*symlen], sizeof(gr_complex) * symlen);
        }
      }

      //std::cout<<"cloud gr-adv: chip to sym work function end"<<std::endl;

      consume_each (noutput_items);

      // Tell runtime system how many output items we produced.
      return noutput_items * symlen;
    }

  } /* namespace advoqpsk */
} /* namespace gr */
