#message($$QMAKESPEC)
QT = widgets

TARGET = translatorJSONQtso
TEMPLATE = lib

!android:QMAKE_CXXFLAGS += -std=c++17
android:CONFIG += c++14
CONFIG += no_keywords plugin

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

!win32:MYPATH = "/"
win32:MYPATH = "H:/veryuseddata/portable/msys64/"

#mine
INCLUDEPATH += $${MYPATH}home/jouven/mylibs/include

QMAKE_CXXFLAGS_DEBUG -= -g
QMAKE_CXXFLAGS_DEBUG += -pedantic -Wall -Wextra -g3

#if not win32, add flto, mingw (on msys2) can't handle lto
linux:QMAKE_CXXFLAGS_RELEASE += -flto=jobserver
!android:QMAKE_CXXFLAGS_RELEASE += -mtune=sandybridge

#for -flto=jobserver in the link step to work with -j4
linux:!android:QMAKE_LINK = +g++

linux:QMAKE_LFLAGS += -fuse-ld=gold
QMAKE_LFLAGS_RELEASE += -fvisibility=hidden
#if not win32, add flto, mingw (on msys2) can't handle lto
linux:QMAKE_LFLAGS_RELEASE += -flto=jobserver

HEADERS += \
    translator.hpp \
    languageLink.hpp


SOURCES += \
    translator.cpp \
    languageLink.cpp


if (!android){
#don't new line the "{"
#release
CONFIG(release, debug|release){

}
#debug
CONFIG(debug, debug|release){

    DEFINES += DEBUGJOUVEN
}
}

if (android){
#release
CONFIG(release, debug|release){

}
#debug
CONFIG(debug, debug|release){

    DEFINES += DEBUGJOUVEN
}

}
