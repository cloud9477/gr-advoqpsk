# CMake generated Testfile for 
# Source directory: /home/cloud/sdr/gr-advoqpsk/python/advoqpsk
# Build directory: /home/cloud/sdr/gr-advoqpsk/build/python/advoqpsk
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(qa_precfofix "/usr/bin/sh" "qa_precfofix_test.sh")
set_tests_properties(qa_precfofix PROPERTIES  _BACKTRACE_TRIPLES "/usr/lib/x86_64-linux-gnu/cmake/gnuradio/GrTest.cmake;119;add_test;/home/cloud/sdr/gr-advoqpsk/python/advoqpsk/CMakeLists.txt;42;GR_ADD_TEST;/home/cloud/sdr/gr-advoqpsk/python/advoqpsk/CMakeLists.txt;0;")
add_test(qa_trigger "/usr/bin/sh" "qa_trigger_test.sh")
set_tests_properties(qa_trigger PROPERTIES  _BACKTRACE_TRIPLES "/usr/lib/x86_64-linux-gnu/cmake/gnuradio/GrTest.cmake;119;add_test;/home/cloud/sdr/gr-advoqpsk/python/advoqpsk/CMakeLists.txt;43;GR_ADD_TEST;/home/cloud/sdr/gr-advoqpsk/python/advoqpsk/CMakeLists.txt;0;")
add_test(qa_sync "/usr/bin/sh" "qa_sync_test.sh")
set_tests_properties(qa_sync PROPERTIES  _BACKTRACE_TRIPLES "/usr/lib/x86_64-linux-gnu/cmake/gnuradio/GrTest.cmake;119;add_test;/home/cloud/sdr/gr-advoqpsk/python/advoqpsk/CMakeLists.txt;44;GR_ADD_TEST;/home/cloud/sdr/gr-advoqpsk/python/advoqpsk/CMakeLists.txt;0;")
add_test(qa_decode "/usr/bin/sh" "qa_decode_test.sh")
set_tests_properties(qa_decode PROPERTIES  _BACKTRACE_TRIPLES "/usr/lib/x86_64-linux-gnu/cmake/gnuradio/GrTest.cmake;119;add_test;/home/cloud/sdr/gr-advoqpsk/python/advoqpsk/CMakeLists.txt;45;GR_ADD_TEST;/home/cloud/sdr/gr-advoqpsk/python/advoqpsk/CMakeLists.txt;0;")
add_test(qa_divider "/usr/bin/sh" "qa_divider_test.sh")
set_tests_properties(qa_divider PROPERTIES  _BACKTRACE_TRIPLES "/usr/lib/x86_64-linux-gnu/cmake/gnuradio/GrTest.cmake;119;add_test;/home/cloud/sdr/gr-advoqpsk/python/advoqpsk/CMakeLists.txt;46;GR_ADD_TEST;/home/cloud/sdr/gr-advoqpsk/python/advoqpsk/CMakeLists.txt;0;")
add_test(qa_powerstep "/usr/bin/sh" "qa_powerstep_test.sh")
set_tests_properties(qa_powerstep PROPERTIES  _BACKTRACE_TRIPLES "/usr/lib/x86_64-linux-gnu/cmake/gnuradio/GrTest.cmake;119;add_test;/home/cloud/sdr/gr-advoqpsk/python/advoqpsk/CMakeLists.txt;47;GR_ADD_TEST;/home/cloud/sdr/gr-advoqpsk/python/advoqpsk/CMakeLists.txt;0;")
subdirs("bindings")
