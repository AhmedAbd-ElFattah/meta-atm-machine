SUMMARY = "Pushbutton recipe"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module


SRC_URI = "file://Pushbutton_CONFIG.h \
           file://Pushbutton.c \ 
           file://Makefile \
           file://COPYING \
           "

S = "${WORKDIR}"
#inherit of module.bbclass will name module packages with "kernel-module-" prefix as required by the oe-core build environment.
RPROVIDES_${PN} += "kernel-module-pmu-user"

KERNEL_MODULE_AUTOLOAD += "Pushbutton"
KERNEL_MODULE_PROBECONF += "Pushbutton"
module_conf_Pushbutton = "options Pushbutton PIN_Num=24"
