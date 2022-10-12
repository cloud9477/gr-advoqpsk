#!/usr/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir="/home/cloud/sdr/gr-advoqpsk/python/advoqpsk"
export GR_CONF_CONTROLPORT_ON=False
export PATH="/home/cloud/sdr/gr-advoqpsk/build/python/advoqpsk":"$PATH"
export LD_LIBRARY_PATH="":$LD_LIBRARY_PATH
export PYTHONPATH=/home/cloud/sdr/gr-advoqpsk/build/test_modules:$PYTHONPATH
/usr/bin/python3 /home/cloud/sdr/gr-advoqpsk/python/advoqpsk/qa_decode.py 
