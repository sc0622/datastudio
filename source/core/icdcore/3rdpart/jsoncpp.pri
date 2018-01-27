HEADERS += \
    $$PWD/jsoncpp/json_tool.h \
    $$PWD/jsoncpp/json/allocator.h \
    $$PWD/jsoncpp/json/assertions.h \
    $$PWD/jsoncpp/json/autolink.h \
    $$PWD/jsoncpp/json/config.h \
    $$PWD/jsoncpp/json/features.h \
    $$PWD/jsoncpp/json/forwards.h \
    $$PWD/jsoncpp/json/json.h \
    $$PWD/jsoncpp/json/reader.h \
    $$PWD/jsoncpp/json/value.h \
    $$PWD/jsoncpp/json/version.h \
    $$PWD/jsoncpp/json/writer.h

SOURCES += \
    $$PWD/jsoncpp/json_reader.cpp \
    $$PWD/jsoncpp/json_value.cpp \
    $$PWD/jsoncpp/json_valueiterator.inl \
    $$PWD/jsoncpp/json_writer.cpp

INCLUDEPATH += $$PWD/jsoncpp

DEFINES += JSON_DLL_BUILD
