SUMMARY = "Linux Device Drivers for ATM Machine Image"

inherit packagegroup

RDEPENDS_${PN} = "available-user-led \
                  keypad \
                  push-button \
                  scrolling-device \
                  seven-segment-module \
                  pulse-buzzer \
                  atm-machine-application"