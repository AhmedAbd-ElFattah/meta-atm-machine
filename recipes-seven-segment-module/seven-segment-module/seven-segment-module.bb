SUMMARY = "Seven Segment Display Module recipe"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module


SRC_URI = "file://SevenSegmentDisplay_CONFIG.h \
           file://SevenSegmentDisplay_PRIVATE.h \
           file://SevenSegmentDisplayModule.c \ 
           file://Makefile \
           file://COPYING \
           "

S = "${WORKDIR}"
#inherit of module.bbclass will name module packages with "kernel-module-" prefix as required by the oe-core build environment.
RPROVIDES_${PN} += "kernel-module-pmu-user"

KERNEL_MODULE_AUTOLOAD += "SevenSegmentDisplayModule"
KERNEL_MODULE_PROBECONF += "SevenSegmentDisplayModule"
module_conf_SevenSegmentDisplayModule = "options SevenSegmentDisplayModule Displays_Count=2 Digits_Count=3 Data_Pin=16 Clock_Pin=20 Enable_Pin=21"

