include(../../coverage.pri)

isEmpty(PREFIX) {
  PREFIX = /usr/local
}

TEMPLATE = aux
DEFINES += "INSTALL_PREFIX=\\\"$$PREFIX\\\""

# Install

install_autopilot.path = $$PREFIX/lib/python2.7/dist-packages/
install_autopilot.files = gallery_app
INSTALLS = install_autopilot
