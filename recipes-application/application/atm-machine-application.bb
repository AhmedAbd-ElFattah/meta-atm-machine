SUMMARY = "Test at program"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

inherit qmake5


DEPENDS += " qtbase wayland "

SRCREV = "${AUTOREV}"
PV = "0.1+GIT${SRCPV}"
SRC_URI = " git://github.com/AhmedAbd-ElFattah/ATM-Machine-GUI.git;;branch=main;protocol=https"

S = "${WORKDIR}/git"



do_install_append(){
    install -d ${D}${bindir}
    install -m 777 ATM-Machine-GUI ${D}${bindir}

    install -d ${D}/etc
    install -m 644 ${S}/ATM-Database.db ${D}/etc/ 
}

FILES_${PN} += "${bindir}/ATM-Machine-GUI"
