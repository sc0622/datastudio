
HEADERS += \
    $$PWD/icdparser_file.h \
    $$PWD/icdparser_file_p.h

SOURCES += \
    $$PWD/icdparser_file.cpp

include($$PWD/json/json.pri)
include($$PWD/xml/xml.pri)
