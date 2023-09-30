SUMMARY = "Keypad recipe"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module


SRC_URI = "file://Keypad_CONFIG.h \
           file://Keypad_PRIVATE.h \
           file://Keypad.c \ 
           file://Makefile \
           file://COPYING \
           "

S = "${WORKDIR}"
#inherit of module.bbclass will name module packages with "kernel-module-" prefix as required by the oe-core build environment.
RPROVIDES_${PN} += "kernel-module-pmu-user"


KERNEL_MODULE_AUTOLOAD += "Keypad"
KERNEL_MODULE_PROBECONF += "Keypad"
module_conf_Keypad = "options Keypad ROW_0=17 ROW_1=27 ROW_2=22 ROW_3=5 COL_0=6 COL_1=13 COL_2=19 COL_3=26"
