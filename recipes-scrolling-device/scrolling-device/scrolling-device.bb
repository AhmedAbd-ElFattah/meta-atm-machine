SUMMARY = "Scrolling Device recipe"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module


SRC_URI = "file://ScrollingDevice_CONFIG.h \
           file://ScrollingDevice.c \ 
           file://Makefile \
           file://COPYING \
           "

S = "${WORKDIR}"
#inherit of module.bbclass will name module packages with "kernel-module-" prefix as required by the oe-core build environment.
RPROVIDES_${PN} += "kernel-module-pmu-user"

KERNEL_MODULE_AUTOLOAD += "ScrollingDevice"
KERNEL_MODULE_PROBECONF += "ScrollingDevice"
module_conf_ScrollingDevice = "options ScrollingDevice PIN_UPPushbutton=2 PIN_DownPushbutton=3"

