#!/bin/sh
LD_LIBRARY_PATH=/home/ubuntu/notkacmd/qtwebsockets/lib:/usr/local/Qt-5.9.2/lib${LD_LIBRARY_PATH:+:$LD_LIBRARY_PATH}
export LD_LIBRARY_PATH
QT_PLUGIN_PATH=/usr/local/Qt-5.9.2/plugins${QT_PLUGIN_PATH:+:$QT_PLUGIN_PATH}
export QT_PLUGIN_PATH
exec "$@"
