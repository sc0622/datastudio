
HEADERS += \
    $$PWD/icdparser_sql.h \
    $$PWD/icdparser_sql_p.h \
    $$PWD/SqlObject.h

SOURCES += \
    $$PWD/icdparser_sql.cpp \
    $$PWD/SqlObject.cpp

include($$PWD/mysql/mysql.pri)
include($$PWD/oracle/oracle.pri)
include($$PWD/sqlite/sqlite.pri)
include($$PWD/sqlserver/sqlserver.pri)
