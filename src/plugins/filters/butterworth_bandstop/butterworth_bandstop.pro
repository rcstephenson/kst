include(../../plugins_sub.pri)

TARGET = $$kstlib(kstplugin_filterbutterworthbandstop)
LIBS += -l$$kstlib(gsl)

SOURCES += \
    butterworth_bandstop.cpp

HEADERS += \
    butterworth_bandstop.h

FORMS += filterbutterworthbandstopconfig.ui
