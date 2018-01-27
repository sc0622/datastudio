HEADERS += \
    $$PWD/icdgenerate.h \
    $$PWD/joffice.h

SOURCES += \
    $$PWD/icdgenerate.cpp \
    $$PWD/joffice.cpp

include($$PWD/csharp/csharp.pri)
include($$PWD/c++/c++.pri)
include($$PWD/excel/excel.pri)
include($$PWD/html/html.pri)
include($$PWD/matlab/matlab.pri)
include($$PWD/pdf/pdf.pri)
include($$PWD/word/word.pri)
