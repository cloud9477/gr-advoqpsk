# gr-advoqpsk
Advanced IEEE 802.15.4 O-QPSK demodulator module for GNU Radio 3.7.

# Dependencies

- GNU Radio (maint-3.7)

# Installation

- GNU Radio installation, please see the [Building and Installing the USRP Open-Source Toolchain (UHD and GNU Radio) on Linux](https://kb.ettus.com/Building_and_Installing_the_USRP_Open-Source_Toolchain_(UHD_and_GNU_Radio)_on_Linux)

- Advanced O-QPSK demodulator installation:
```
cd gr-advoqpsk
mkdir build
cd build
cmake ../
make
sudo make install
sudo ldconfig
```

# Version
- v1.0: the first release verison.
- maint-3.7: the first version and buffer inside each module is removed speeding up the processing.

# Performance results from GNU Radio control port
- Average clock usage of modules with buffer:
![alt text](https://github.com/cloud9477/gr-advoqpsk/blob/main/perfAdvBuf.png?raw=true)
- Average clock usage of modules without buffer:
![alt text](https://github.com/cloud9477/gr-advoqpsk/blob/main/perfAdvNoBuf.png?raw=true)
- The modules without buffers reduces around 90% processing time.
- The processing in Advanced IEEE 802.15.4 O-QPSK uses double in all the moduels, so the moduels are not compatible with original GNU Radio blocks. We keep the double since in our test, double uses less processing time, around 50% of the float.

# Data link layer stack

In the example folder, we provide an example using the data link layer stack from [WIME Project](https://www.wime-project.net/). For the installation, please see the details on their website or [gr-ieee802-15-4](https://github.com/bastibl/gr-ieee802-15-4).

