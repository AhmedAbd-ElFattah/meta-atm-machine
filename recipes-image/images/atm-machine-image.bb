DESCRIPTION = "Image for ATM-Machine"

require recipes-sato/images/core-image-sato.bb

IMAGE_INSTALL_append += " atm-machine-application sqlite3 " 

IMAGE_INSTALL_append += " qtbase qtwayland ttf-bitstream-vera "
TOOLCHAIN_TARGET_TASK += " libgl-mesa-dev "
CORE_IMAGE_BASE_INSTALL += " wayland weston "

PACKAGECONFIG[sql-sqlite] = "-sql-sqlite -system-sqlite,-no-sql-sqlite,sqlite3"
PACKAGECONFIG_append = " sql-sqlite"


IMAGE_FSTYPES += "wic wic.bmap"
IMAGE_INSTALL_append = " packagegroups-device-drivers"