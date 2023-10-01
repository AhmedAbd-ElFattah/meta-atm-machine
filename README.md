# meta-atm-machine

This yocto layer is is responsible for compiling the [ATM application](https://github.com/AhmedAbd-ElFattah/ATM-Machine-GUI) and additional Linux device drivers.

## Getting started
1. Install Build Host Packages of yocto dunfell [Reference.](https://docs.yoctoproject.org/3.1.28/brief-yoctoprojectqs/brief-yoctoprojectqs.html#build-host-packages)

```
sudo apt-get install gawk wget git-core diffstat unzip texinfo gcc-multilib build-essential chrpath socat cpio python3 python3-pip python3-pexpect xz-utils debianutils iputils-ping python3-git python3-jinja2 libegl1-mesa libsdl1.2-dev pylint3 xterm python3-subunit mesa-common-dev
```

2. Select machine through
```bash
export MACHINE="raspberrypi3"
```

3. Clone yocto project dunfell branch
```bash
git clone -b dunfell git://git.yoctoproject.org/poky
cd poky
```

4.  Download the following layers
Note that one of resposibilities of meta-atm-machine layer is fetch application through git repository
```bash
git clone https://github.com/AhmedAbd-ElFattah/meta-atm-machine
git clone -b dunfell https://github.com/meta-qt5/meta-qt5
git clone -b dunfell git://git.openembedded.org/meta-openembedded
git clone -b dunfell git://git.yoctoproject.org/meta-raspberrypi
git clone -b dunfell https://github.com/schnitzeltony/meta-qt5-extra
```

5. run the oe-init-build-env environment setup script
```bash
source oe-init-build-env ATM-Image
```

6. Add theese layers

```bash
bitbake-layers add-layer ../meta-openembedded/meta-oe
bitbake-layers add-layer ../meta-openembedded/meta-python
bitbake-layers add-layer ../meta-openembedded/meta-networking
bitbake-layers add-layer ../meta-openembedded/meta-multimedia
bitbake-layers add-layer ../meta-openembedded/meta-gnome
bitbake-layers add-layer ../meta-raspberrypi
bitbake-layers add-layer ../meta-qt5
bitbake-layers add-layer ../meta-qt5-extra
bitbake-layers add-layer ../meta-atm-machine
```
7. Build Image
```bash
bitbake atm-machine-image
```

8. Flash image into SD-Card
You can use [this link](https://docs.yoctoproject.org/dev/dev-manual/bmaptool.html) from yocto to help you flashing image into SD-Card
