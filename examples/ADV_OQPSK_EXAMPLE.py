#!/usr/bin/env python3
# -*- coding: utf-8 -*-

#
# SPDX-License-Identifier: GPL-3.0
#
# GNU Radio Python Flow Graph
# Title: Not titled yet
# GNU Radio version: 3.10.4.0

from packaging.version import Version as StrictVersion

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print("Warning: failed to XInitThreads()")

from gnuradio import advoqpsk
from gnuradio import blocks
import pmt
from gnuradio import gr
from gnuradio.filter import firdes
from gnuradio.fft import window
import sys
import signal
from PyQt5 import Qt
from argparse import ArgumentParser
from gnuradio.eng_arg import eng_float, intx
from gnuradio import eng_notation
from gnuradio import gr, pdu
from gnuradio import network



from gnuradio import qtgui

class ADV_OQPSK_EXAMPLE(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Not titled yet", catch_exceptions=True)
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Not titled yet")
        qtgui.util.check_set_qss()
        try:
            self.setWindowIcon(Qt.QIcon.fromTheme('gnuradio-grc'))
        except:
            pass
        self.top_scroll_layout = Qt.QVBoxLayout()
        self.setLayout(self.top_scroll_layout)
        self.top_scroll = Qt.QScrollArea()
        self.top_scroll.setFrameStyle(Qt.QFrame.NoFrame)
        self.top_scroll_layout.addWidget(self.top_scroll)
        self.top_scroll.setWidgetResizable(True)
        self.top_widget = Qt.QWidget()
        self.top_scroll.setWidget(self.top_widget)
        self.top_layout = Qt.QVBoxLayout(self.top_widget)
        self.top_grid_layout = Qt.QGridLayout()
        self.top_layout.addLayout(self.top_grid_layout)

        self.settings = Qt.QSettings("GNU Radio", "ADV_OQPSK_EXAMPLE")

        try:
            if StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
                self.restoreGeometry(self.settings.value("geometry").toByteArray())
            else:
                self.restoreGeometry(self.settings.value("geometry"))
        except:
            pass

        ##################################################
        # Variables
        ##################################################
        self.threshold = threshold = 0.3

        ##################################################
        # Blocks
        ##################################################
        self.pdu_pdu_to_tagged_stream_0 = pdu.pdu_to_tagged_stream(gr.types.byte_t, 'packet_len')
        self.network_udp_sink_0 = network.udp_sink(gr.sizeof_char, 1, '127.0.0.1', 2000, 0, 1472, False)
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*1, '/home/cloud/sdr/gr-advoqpsk/examples/ieee80211_oqpsk_sig_30.bin', False, 0, 0)
        self.blocks_file_source_0.set_begin_tag(pmt.PMT_NIL)
        self.advoqpsk_trigger_3 = advoqpsk.trigger(threshold * 0.8)
        self.advoqpsk_trigger_2 = advoqpsk.trigger(threshold * 0.8)
        self.advoqpsk_trigger_1 = advoqpsk.trigger(threshold * 0.8)
        self.advoqpsk_trigger_0 = advoqpsk.trigger(threshold * 0.8)
        self.advoqpsk_sync_0 = advoqpsk.sync(threshold)
        self.advoqpsk_precfofix_1 = advoqpsk.precfofix(83)
        self.advoqpsk_precfofix_0 = advoqpsk.precfofix(250)
        self.advoqpsk_powerstep_0 = advoqpsk.powerstep()
        self.advoqpsk_divider_0 = advoqpsk.divider()
        self.advoqpsk_decode_0 = advoqpsk.decode(True)


        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.advoqpsk_decode_0, 'out'), (self.pdu_pdu_to_tagged_stream_0, 'pdus'))
        self.connect((self.advoqpsk_divider_0, 0), (self.advoqpsk_decode_0, 0))
        self.connect((self.advoqpsk_divider_0, 1), (self.advoqpsk_powerstep_0, 0))
        self.connect((self.advoqpsk_divider_0, 0), (self.advoqpsk_precfofix_0, 0))
        self.connect((self.advoqpsk_divider_0, 0), (self.advoqpsk_precfofix_1, 0))
        self.connect((self.advoqpsk_divider_0, 0), (self.advoqpsk_sync_0, 0))
        self.connect((self.advoqpsk_powerstep_0, 0), (self.advoqpsk_sync_0, 1))
        self.connect((self.advoqpsk_powerstep_0, 0), (self.advoqpsk_trigger_0, 1))
        self.connect((self.advoqpsk_powerstep_0, 0), (self.advoqpsk_trigger_1, 1))
        self.connect((self.advoqpsk_powerstep_0, 0), (self.advoqpsk_trigger_2, 1))
        self.connect((self.advoqpsk_powerstep_0, 0), (self.advoqpsk_trigger_3, 1))
        self.connect((self.advoqpsk_precfofix_0, 0), (self.advoqpsk_trigger_0, 0))
        self.connect((self.advoqpsk_precfofix_0, 1), (self.advoqpsk_trigger_1, 0))
        self.connect((self.advoqpsk_precfofix_1, 0), (self.advoqpsk_trigger_2, 0))
        self.connect((self.advoqpsk_precfofix_1, 1), (self.advoqpsk_trigger_3, 0))
        self.connect((self.advoqpsk_sync_0, 1), (self.advoqpsk_decode_0, 2))
        self.connect((self.advoqpsk_sync_0, 0), (self.advoqpsk_decode_0, 1))
        self.connect((self.advoqpsk_trigger_0, 0), (self.advoqpsk_sync_0, 2))
        self.connect((self.advoqpsk_trigger_1, 0), (self.advoqpsk_sync_0, 3))
        self.connect((self.advoqpsk_trigger_2, 0), (self.advoqpsk_sync_0, 4))
        self.connect((self.advoqpsk_trigger_3, 0), (self.advoqpsk_sync_0, 5))
        self.connect((self.blocks_file_source_0, 0), (self.advoqpsk_divider_0, 0))
        self.connect((self.pdu_pdu_to_tagged_stream_0, 0), (self.network_udp_sink_0, 0))


    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "ADV_OQPSK_EXAMPLE")
        self.settings.setValue("geometry", self.saveGeometry())
        self.stop()
        self.wait()

        event.accept()

    def get_threshold(self):
        return self.threshold

    def set_threshold(self, threshold):
        self.threshold = threshold




def main(top_block_cls=ADV_OQPSK_EXAMPLE, options=None):

    if StrictVersion("4.5.0") <= StrictVersion(Qt.qVersion()) < StrictVersion("5.0.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()

    tb.start()

    tb.show()

    def sig_handler(sig=None, frame=None):
        tb.stop()
        tb.wait()

        Qt.QApplication.quit()

    signal.signal(signal.SIGINT, sig_handler)
    signal.signal(signal.SIGTERM, sig_handler)

    timer = Qt.QTimer()
    timer.start(500)
    timer.timeout.connect(lambda: None)

    qapp.exec_()

if __name__ == '__main__':
    main()
