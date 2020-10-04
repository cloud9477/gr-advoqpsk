/* -*- c++ -*- */

#define ADVOQPSK_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "advoqpsk_swig_doc.i"

%{
#include "advoqpsk/advoqpsk_divider.h"
#include "advoqpsk/advoqpsk_powerstep.h"
#include "advoqpsk/advoqpsk_precfofix.h"
#include "advoqpsk/advoqpsk_trigger.h"
#include "advoqpsk/advoqpsk_sync.h"
#include "advoqpsk/advoqpsk_decode.h"
%}


%include "advoqpsk/advoqpsk_divider.h"
GR_SWIG_BLOCK_MAGIC2(advoqpsk, advoqpsk_divider);
%include "advoqpsk/advoqpsk_powerstep.h"
GR_SWIG_BLOCK_MAGIC2(advoqpsk, advoqpsk_powerstep);
%include "advoqpsk/advoqpsk_precfofix.h"
GR_SWIG_BLOCK_MAGIC2(advoqpsk, advoqpsk_precfofix);
%include "advoqpsk/advoqpsk_trigger.h"
GR_SWIG_BLOCK_MAGIC2(advoqpsk, advoqpsk_trigger);
%include "advoqpsk/advoqpsk_sync.h"
GR_SWIG_BLOCK_MAGIC2(advoqpsk, advoqpsk_sync);
%include "advoqpsk/advoqpsk_decode.h"
GR_SWIG_BLOCK_MAGIC2(advoqpsk, advoqpsk_decode);
