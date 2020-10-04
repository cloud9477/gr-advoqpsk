# gr-advoqpsk
Advanced O-QPSK demodulator module for GNU Radio.

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
make -o3
sudo make install
sudo ldconfig
```

# Data link layer stack

In the example folder, we provide an example using the data link layer stack from [WIME Project](https://www.wime-project.net/). For the installation, please see the details on their website or [gr-ieee802-15-4](https://github.com/bastibl/gr-ieee802-15-4).

