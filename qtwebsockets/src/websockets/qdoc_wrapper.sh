#!/bin/sh
QT_VERSION=5.9.2
export QT_VERSION
QT_VER=5.9
export QT_VER
QT_VERSION_TAG=592
export QT_VERSION_TAG
QT_INSTALL_DOCS=/usr/local/Qt-5.9.2/doc
export QT_INSTALL_DOCS
BUILDDIR=/home/ubuntu/notkacmd/qtwebsockets/src/websockets
export BUILDDIR
exec /usr/local/Qt-5.9.2/bin/qdoc "$@"
