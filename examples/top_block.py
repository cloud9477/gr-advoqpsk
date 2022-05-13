#!/usr/bin/env python2
# -*- coding: utf-8 -*-
##################################################
# GNU Radio Python Flow Graph
# Title: Top Block
# GNU Radio version: 3.7.14.0
##################################################

if __name__ == '__main__':
    import ctypes
    import sys
    if sys.platform.startswith('linux'):
        try:
            x11 = ctypes.cdll.LoadLibrary('libX11.so')
            x11.XInitThreads()
        except:
            print "Warning: failed to XInitThreads()"

from PyQt4 import Qt
from gnuradio import blocks
from gnuradio import eng_notation
from gnuradio import gr
from gnuradio.ctrlport.monitor import *
from gnuradio.eng_option import eng_option
from gnuradio.filter import firdes
from optparse import OptionParser
import advoqpsk
import pmt
import sys
from gnuradio import qtgui


class top_block(gr.top_block, Qt.QWidget):

    def __init__(self):
        gr.top_block.__init__(self, "Top Block")
        Qt.QWidget.__init__(self)
        self.setWindowTitle("Top Block")
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

        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.restoreGeometry(self.settings.value("geometry").toByteArray())


        ##################################################
        # Variables
        ##################################################
        self.threshold = threshold = 0.3
        self.samp_rate = samp_rate = 4e6

        ##################################################
        # Blocks
        ##################################################
        self.blocks_udp_sink_0 = blocks.udp_sink(gr.sizeof_char*1, '127.0.0.1', 8081, 1472, True)
        self.blocks_throttle_0 = blocks.throttle(gr.sizeof_gr_complex*1, samp_rate,True)
        self.blocks_pdu_to_tagged_stream_0 = blocks.pdu_to_tagged_stream(blocks.byte_t, 'pdu_length')
        self.blocks_file_source_0 = blocks.file_source(gr.sizeof_gr_complex*1, '/home/cloud/advTf_30_100', True)
        self.blocks_file_source_0.set_begin_tag(pmt.PMT_NIL)
        self.blocks_ctrlport_monitor_performance_0 = not True or monitor("gr-perf-monitorx")
        self.blocks_ctrlport_monitor_0 = not True or monitor()
        self.advoqpsk_advoqpsk_trigger_0_2 = advoqpsk.advoqpsk_trigger(threshold*0.8)
        self.advoqpsk_advoqpsk_trigger_0_1 = advoqpsk.advoqpsk_trigger(threshold*0.8)
        self.advoqpsk_advoqpsk_trigger_0_0 = advoqpsk.advoqpsk_trigger(threshold*0.8)
        self.advoqpsk_advoqpsk_trigger_0 = advoqpsk.advoqpsk_trigger(threshold*0.8)
        self.advoqpsk_advoqpsk_sync_0 = advoqpsk.advoqpsk_sync(threshold)
        self.advoqpsk_advoqpsk_precfofix_0_0_0 = advoqpsk.advoqpsk_precfofix(83)
        self.advoqpsk_advoqpsk_precfofix_0 = advoqpsk.advoqpsk_precfofix(250)
        self.advoqpsk_advoqpsk_powerstep_0 = advoqpsk.advoqpsk_powerstep()
        self.advoqpsk_advoqpsk_divider_0 = advoqpsk.advoqpsk_divider()
        self.advoqpsk_advoqpsk_decode_0 = advoqpsk.advoqpsk_decode(False)



        ##################################################
        # Connections
        ##################################################
        self.msg_connect((self.advoqpsk_advoqpsk_decode_0, 'out'), (self.blocks_pdu_to_tagged_stream_0, 'pdus'))
        self.connect((self.advoqpsk_advoqpsk_divider_0, 0), (self.advoqpsk_advoqpsk_decode_0, 0))
        self.connect((self.advoqpsk_advoqpsk_divider_0, 1), (self.advoqpsk_advoqpsk_powerstep_0, 0))
        self.connect((self.advoqpsk_advoqpsk_divider_0, 0), (self.advoqpsk_advoqpsk_precfofix_0, 0))
        self.connect((self.advoqpsk_advoqpsk_divider_0, 0), (self.advoqpsk_advoqpsk_precfofix_0_0_0, 0))
        self.connect((self.advoqpsk_advoqpsk_divider_0, 0), (self.advoqpsk_advoqpsk_sync_0, 0))
        self.connect((self.advoqpsk_advoqpsk_powerstep_0, 0), (self.advoqpsk_advoqpsk_sync_0, 1))
        self.connect((self.advoqpsk_advoqpsk_powerstep_0, 0), (self.advoqpsk_advoqpsk_trigger_0, 1))
        self.connect((self.advoqpsk_advoqpsk_powerstep_0, 0), (self.advoqpsk_advoqpsk_trigger_0_0, 1))
        self.connect((self.advoqpsk_advoqpsk_powerstep_0, 0), (self.advoqpsk_advoqpsk_trigger_0_1, 1))
        self.connect((self.advoqpsk_advoqpsk_powerstep_0, 0), (self.advoqpsk_advoqpsk_trigger_0_2, 1))
        self.connect((self.advoqpsk_advoqpsk_precfofix_0, 1), (self.advoqpsk_advoqpsk_trigger_0_1, 0))
        self.connect((self.advoqpsk_advoqpsk_precfofix_0, 0), (self.advoqpsk_advoqpsk_trigger_0_2, 0))
        self.connect((self.advoqpsk_advoqpsk_precfofix_0_0_0, 1), (self.advoqpsk_advoqpsk_trigger_0, 0))
        self.connect((self.advoqpsk_advoqpsk_precfofix_0_0_0, 0), (self.advoqpsk_advoqpsk_trigger_0_0, 0))
        self.connect((self.advoqpsk_advoqpsk_sync_0, 1), (self.advoqpsk_advoqpsk_decode_0, 2))
        self.connect((self.advoqpsk_advoqpsk_sync_0, 0), (self.advoqpsk_advoqpsk_decode_0, 1))
        self.connect((self.advoqpsk_advoqpsk_trigger_0, 0), (self.advoqpsk_advoqpsk_sync_0, 5))
        self.connect((self.advoqpsk_advoqpsk_trigger_0_0, 0), (self.advoqpsk_advoqpsk_sync_0, 4))
        self.connect((self.advoqpsk_advoqpsk_trigger_0_1, 0), (self.advoqpsk_advoqpsk_sync_0, 3))
        self.connect((self.advoqpsk_advoqpsk_trigger_0_2, 0), (self.advoqpsk_advoqpsk_sync_0, 2))
        self.connect((self.blocks_file_source_0, 0), (self.blocks_throttle_0, 0))
        self.connect((self.blocks_pdu_to_tagged_stream_0, 0), (self.blocks_udp_sink_0, 0))
        self.connect((self.blocks_throttle_0, 0), (self.advoqpsk_advoqpsk_divider_0, 0))

    def closeEvent(self, event):
        self.settings = Qt.QSettings("GNU Radio", "top_block")
        self.settings.setValue("geometry", self.saveGeometry())
        event.accept()

    def get_threshold(self):
        return self.threshold

    def set_threshold(self, threshold):
        self.threshold = threshold

    def get_samp_rate(self):
        return self.samp_rate

    def set_samp_rate(self, samp_rate):
        self.samp_rate = samp_rate
        self.blocks_throttle_0.set_sample_rate(self.samp_rate)


def main(top_block_cls=top_block, options=None):

    from distutils.version import StrictVersion
    if StrictVersion(Qt.qVersion()) >= StrictVersion("4.5.0"):
        style = gr.prefs().get_string('qtgui', 'style', 'raster')
        Qt.QApplication.setGraphicsSystem(style)
    qapp = Qt.QApplication(sys.argv)

    tb = top_block_cls()
    tb.start()
    tb.show()

    def quitting():
        tb.stop()
        tb.wait()
    qapp.connect(qapp, Qt.SIGNAL("aboutToQuit()"), quitting)
    if True:
        if True:
            (tb.blocks_ctrlport_monitor_0).start()
    else:
        sys.stderr.write("Monitor '{0}' does not have an enable ('en') parameter.".format("tb.blocks_ctrlport_monitor_0"))
    if True:
        if True:
            (tb.blocks_ctrlport_monitor_performance_0).start()
    else:
        sys.stderr.write("Monitor '{0}' does not have an enable ('en') parameter.".format("tb.blocks_ctrlport_monitor_performance_0"))
    qapp.exec_()


if __name__ == '__main__':
    main()
