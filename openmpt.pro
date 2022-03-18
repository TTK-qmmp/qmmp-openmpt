include($$PWD/../../plugins.pri)

TARGET = $$PLUGINS_PREFIX/Input/openmpt

HEADERS += decoderopenmptfactory.h \
           decoder_openmpt.h \
           openmptmetadatamodel.h \
           openmpthelper.h \
           archivereader.h \
           settingsdialog.h

SOURCES += decoderopenmptfactory.cpp \
           decoder_openmpt.cpp \
           openmptmetadatamodel.cpp \
           openmpthelper.cpp \
           archivereader.cpp \
           settingsdialog.cpp

FORMS   += settingsdialog.ui

unix {
    target.path = $$PLUGIN_DIR/Input
    INSTALLS += target
    PKGCONFIG += libopenmpt
    QMAKE_CLEAN = $$PLUGINS_PREFIX/Input/libopenmpt.so
}

win32 {
    LIBS += -lopenmpt
}
