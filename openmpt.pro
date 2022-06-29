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

#CONFIG += BUILD_PLUGIN_INSIDE
contains(CONFIG, BUILD_PLUGIN_INSIDE){
    include($$PWD/../../plugins.pri)
    TARGET = $$PLUGINS_PREFIX/Input/openmpt

    unix{
        target.path = $$PLUGIN_DIR/Input
        INSTALLS += target
        PKGCONFIG += libopenmpt
    }

    win32{
        LIBS += -lopenmpt
    }
}else{
    QT += widgets
    CONFIG += warn_off plugin lib thread link_pkgconfig c++11
    TEMPLATE = lib

    unix{
        PKGCONFIG += qmmp-1 libopenmpt

        PLUGIN_DIR = $$system(pkg-config qmmp-1 --variable=plugindir)/Input
        INCLUDEPATH += $$system(pkg-config qmmp-1 --variable=prefix)/include

        plugin.path = $${PLUGIN_DIR}
        plugin.files = lib$${TARGET}.so
        INSTALLS += plugin
    }
}
