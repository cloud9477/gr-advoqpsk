# gr-advoqpsk
- Advanced IEEE 802.15.4 O-QPSK demodulator module.
- The previous version was based on GNU Radio 3.7, please checkout to maint-3.7 branch.
- The current version (main branch) is based on GNU Radio 3.10 with Ubuntu 20.04 or Ubuntu 22.04.

# Dependencies

- GNU Radio 3.7 or 3.10.
- Compatible to the data link layer of [WIME Project](https://www.wime-project.net/).

# Installation

- GNU Radio installation from the source code, please see the [Building and Installing the USRP Open-Source Toolchain (UHD and GNU Radio) on Linux](https://kb.ettus.com/Building_and_Installing_the_USRP_Open-Source_Toolchain_(UHD_and_GNU_Radio)_on_Linux).
- For Ubuntu user, we recommand to install the GNU Radio with apt, see: [InstallingGR](https://wiki.gnuradio.org/index.php/InstallingGR).
- Please notice that during the installation, it should be the gnuradio-dev to complile and use OOT modules but not gnuradio.
- Ubuntu 20.04:
```
sudo add-apt-repository ppa:gnuradio/gnuradio-releases
sudo apt-get update
sudo apt-get install gnuradio-dev python3-packaging
```
- Ubuntu 22.04:
```
sudo apt-get install gnuradio-dev
```
- To use USRP with GNU Radio, install the uhd-host:
```
sudo apt-get install uhd-host
sudo cp /lib/uhd/utils/uhd-usrp.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules
sudo udevadm trigger
sudo uhd_images_downloader
```
- Advanced O-QPSK demodulator installation:
```
git clone https://github.com/cloud9477/gr-advoqpsk.git
cd gr-advoqpsk
mkdir build
cd build
cmake ../
make
sudo make install
sudo ldconfig
```
- After the installation, you can open the 0_advoqpsk_binFileDemod.grc in the examples folder to demodulate a recorded signal file in the same folder. Please remember to change the file path in the block "file source".
- To use it with the WIME data link layer, please also install the following two modules, gr-foo and gr-ieee802-15-4.
- Install the libsnd first:
```
sudo apt-get install libsndfile1-dev
```
- And then:
```
git clone https://github.com/bastibl/gr-foo.git
cd gr-foo
git checkout maint-3.10
mkdir build
cd build
cmake ../
make
sudo make install
sudo ldconfig
cd ../../

git clone https://github.com/bastibl/gr-ieee802-15-4.git
cd gr-ieee802-11
git checkout maint-3.10
mkdir build
cd build
cmake ../
make
sudo make install
sudo ldconfig
```
- After the installation, open another grc file 1_ieee802154_oqpsk.grc which is the physical layer hier block with the advanced receiver and click the "generate the flow graph". Next, copy the genrated ieee802_15_4_oqpsk_phy.py and ieee802_15_4_oqpsk_phy.block.yml to a hidden folder named .grc_gnuradio in your home folder. Then re-open the gnuradio-companion and open the other example 2_transceiver_oqpsk.grc. Run the grc for the self-loop example. Finally, you can use it with the USRP to communicate with other machines to form a network. Run the 3_transceiver_oqpsk_usrp.grc. This creates a wireless node with MAC layer interface, here we use the UDP. You can use the command "nc" to create an UDP server to listen to packets and an UDP client to send packets, as shown in the figure below. The command "nc -u 127.0.0.1 9527" creates an UDP client to send your message to the UDP server in the GNU Radio and the Socket PDU block sends the packet to the MAC layer and finally the physical layer. And the same, the "nc -ul 127.0.0.1 9528" creates an UDP server to listen the packets from the MAC layer through the other PDU Socket block.
![alt text](https://github.com/cloud9477/gr-advoqpsk/blob/main/transceiver.png?raw=true)
# Version
- v1.0: the first release verison.
- maint-3.7: the first version and buffer inside each module is removed speeding up the processing.

# Performance results from GNU Radio control port
- Average clock usage of Advanced O-QPSK modules with buffer:
![alt text](https://github.com/cloud9477/gr-advoqpsk/blob/main/perfAdvBuf.png?raw=true)
- Average clock usage of Advanced O-QPSK modules without buffer:
![alt text](https://github.com/cloud9477/gr-advoqpsk/blob/main/perfAdvNoBuf.png?raw=true)
- Average clock usage of WIME O-QPSK modules:
![alt text](https://github.com/cloud9477/gr-advoqpsk/blob/main/perfWime.png?raw=true)
- The modules in Advanced O-QPSK without buffers reduces around 90% processing time.
- The processing in Advanced IEEE 802.15.4 O-QPSK uses double in all the moduels, so the moduels are not compatible with original GNU Radio blocks. We keep the double since in our test, double uses less processing time, around 50% of the float.
- In our test, the total average clock of Advanced O-QPSK is around 216800, the WIME O-QPSK is around 57500, that is around 4 times of the processing time, but the Advanced O-QPSK performance is around 10 dB better than the WIME O-QPSK (see the figure below).
![alt text](https://github.com/cloud9477/gr-advoqpsk/blob/main/perfCompare.png?raw=true)

# Data link layer stack
- In the example folder, we provide an example using the data link layer stack from [WIME Project](https://www.wime-project.net/). For the installation, please see the details on their website or [gr-ieee802-15-4](https://github.com/bastibl/gr-ieee802-15-4).

# Citation
- Faulkner, E., Yun, Z., Zhou, S., Shi, Z.J., Han, S. and Giannakis, G.B., 2021. An advanced GNU radio receiver of IEEE 802.15.4 OQPSK physical layer. IEEE Internet of Things Journal, 8(11), pp.9206-9218.
