#-------------------------------------------------
#
# Project created by QtCreator 2011-01-26T08:54:56
#
#-------------------------------------------------

QT       += core gui

TARGET = PokéRNG
TEMPLATE = app


SOURCES += main.cpp\
        mainwidget.cpp \
    searchthread.cpp \
    pokemonrng.cpp \
    spreadtablemodel.cpp \
    spreadsortmodel.cpp \
    mersennetwister.cpp \
    spreadinfo.cpp \
    spreadtableview.cpp \
    encounterframes.cpp \
    encslotdialog.cpp

HEADERS  += mainwidget.h \
    searchthread.h \
    spread.h \
    pokemonrng.h \
    spreadsearch.h \
    spreadtablemodel.h \
    spreadsortmodel.h \
    mersennetwister.h \
    spreadinfo.h \
    spreadtableview.h \
    encounterframes.h \
    encslotdialog.h

FORMS    += mainwidget.ui \
    spreadinfo.ui \
    encslotdialog.ui

ICON = iconfile.icns

QMAKE_CXXFLAGS += -O3 \
    -funroll-loops
