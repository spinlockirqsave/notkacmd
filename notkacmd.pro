QT += core widgets websockets sql
QT -= gui

CONFIG += c++14

TARGET = notkacmd
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
    src/endpointwebsocket.cpp \
    src/globals.cpp \
    src/main.cpp \
    src/msg.cpp \
    src/msghandler.cpp \
    src/notka.cpp \
    src/task.cpp \
    src/websocketsession.cpp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

HEADERS += \
    inc/websocketsession.h \
    inc/msghandler.h \
    inc/notka.h \
    inc/task.h \
    inc/globals.h \
    inc/endpointwebsocket.h \
    inc/notkaendpoint.h \
    inc/msg.h

DISTFILES += \
    src/db.sh \
    html/notkawebclient/src/App.js \
    html/notkawebclient/src/App.test.js \
    html/notkawebclient/src/FormLogin.js \
    html/notkawebclient/src/FormNotka.js \
    html/notkawebclient/src/FormRegister.js \
    html/notkawebclient/src/HelloGreeting.js \
    html/notkawebclient/src/index.js \
    html/notkawebclient/src/registerServiceWorker.js \
    html/notkawebclient/src/Websocket.js \
    html/notkawebclient/public/manifest.json \
    html/notkawebclient/package.json \
    html/notkawebclient/public/pencil.png \
    html/notkawebclient/src/logo.svg \
    html/notkawebclient/public/pencil.ico \
    html/notkawebclient/src/App.css \
    html/notkawebclient/src/FormLogin.css \
    html/notkawebclient/src/FormNotka.css \
    html/notkawebclient/src/FormRegister.css \
    html/notkawebclient/src/index.css \
    html/notkawebclient/public/index.html \
    html/webclient.html \
    html/notkawebclient/README.md
