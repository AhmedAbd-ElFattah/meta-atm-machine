SUMMARY = "Pulse Buzzer Driver recipe"
LICENSE = "GPLv2"
LIC_FILES_CHKSUM = "file://COPYING;md5=12f884d2ae1ff87c09e5b7ccc2c4ca7e"

inherit module


SRC_URI = "file://PulseBuzzer_CONFIG.h \
           file://PulseBuzzer.c \ 
           file://Makefile \
           file://COPYING \
           "

S = "${WORKDIR}"
#inherit of module.bbclass will name module packages with "kernel-module-" prefix as required by the oe-core build environment.
RPROVIDES_${PN} += "kernel-module-pmu-user"

KERNEL_MODULE_AUTOLOAD += "PulseBuzzer"
KERNEL_MODULE_PROBECONF += "PulseBuzzer"
module_conf_PulseBuzzer = "options PulseBuzzer PulseBuzzer_Pin=12"
