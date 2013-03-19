include(../coverage.pri)

isEmpty(PREFIX) {
  PREFIX = /usr/local
}

TEMPLATE = aux

DEFINES += "INSTALL_PREFIX=\\\"$$PREFIX\\\""
QMAKE_RESOURCE_FLAGS += -root .

# Install

install_resources.path = $$PREFIX/share/gallery-app/
install_resources.files = .
INSTALLS = install_resources

