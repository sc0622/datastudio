HEADERS += \
    $$PWD/commdetail_widget.h \
    $$PWD/commgr_widget_p.h

SOURCES += \
    $$PWD/commdetail_widget.cpp \
    $$PWD/commgr_widget_p.cpp

include($$PWD/file/file.pri)
include($$PWD/serial/serial.pri)
include($$PWD/udp/udp.pri)
