HEADERS += decoderopenmptfactory.h \
           decoder_openmpt.h \
           openmptmetadatamodel.h \
           openmpthelper.h \
           settingsdialog.h

SOURCES += decoderopenmptfactory.cpp \
           decoder_openmpt.cpp \
           openmptmetadatamodel.cpp \
           openmpthelper.cpp \
           settingsdialog.cpp

FORMS += settingsdialog.ui

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
        equals(QT_MAJOR_VERSION, 5){
            QMMP_PKG = qmmp-1
        }else:equals(QT_MAJOR_VERSION, 6){
            QMMP_PKG = qmmp
        }else{
            error("Unsupported Qt version: 5 or 6 is required")
        }
        
        PKGCONFIG += $${QMMP_PKG} libopenmpt

        PLUGIN_DIR = $$system(pkg-config $${QMMP_PKG} --variable=plugindir)/Input
        INCLUDEPATH += $$system(pkg-config $${QMMP_PKG} --variable=prefix)/include

        plugin.path = $${PLUGIN_DIR}
        plugin.files = lib$${TARGET}.so
        INSTALLS += plugin
    }
}
