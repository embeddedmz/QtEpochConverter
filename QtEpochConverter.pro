TARGET = QtEpochConverter
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .
QT += gui core declarative

SOURCES += main.cpp\
        epochconverter.cpp

HEADERS  += epochconverter.h \
    strptime.h

FORMS    += epochconverter.ui

RESOURCES += \
    Icons.qrc

win32:RC_ICONS += calendar_clock.ico

CONFIG(release, debug|release): CONFIG += release
CONFIG(debug, debug|release): CONFIG += debug
