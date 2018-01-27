#-------------------------------------------------
# Project created by QtCreator 2016-07-18T18:18:11
#-------------------------------------------------

this_dir = $$PWD/../../..

QT += gui xml

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += printsupport
}

win32 {
    QT += axcontainer
}

DEFINES += \
    PRO_CORE \
    ICDPARSER_LIB \
    ICDPARSER_BUILD \
    ICDCORE_LIB \
    ICDWORKER_LIB \
    KERNELCLASS_LIB \
    DBACCESS_LIB

## TEST
#DEFINES += PARSE_TABLE_DYNAMIC_ID

include($$this_dir/source/common/build.pri)

TARGET = $$qtLibraryTarget(icdparser)
#CONFIG += staticlib     ## static library

# The .h file which was generated for your project. Feel free to hack it.
HEADERS += \
    icdparser.h \
    icdparser_global.h \
    icdparser_inc.h \
    icdparser_sql.h \
    icdparser_file.h \
    private/icdparser_file_p.h \
    private/icdparser_sql_p.h \
    private/generate/icdgenerate.h \
    private/generate/c++/icdgenerator_cpp.h \
    private/generate/c++/icdgenerator_cpp_p.h \
    private/generate/word/icdgenerator_word.h \
    private/generate/word/icdgenerator_word_p.h \
    private/generate/joffice.h \
    private/generate/word/jword.h \
    private/generate/html/icdgenerator_html.h \
    private/generate/html/icdgenerator_html_p.h \
    private/generate/pdf/icdgenerator_pdf.h \
    private/generate/pdf/icdgenerator_pdf_p.h \
    private/json/icdparser_json.h \
    private/xml/icdparser_xml.h \


# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += \
    icdparser.cpp \
    icdparser_file.cpp \
    icdparser_sql.cpp \
    private/icdparser_sql_save.cpp \
    private/icdparser_sql_restore.cpp \
    private/generate/icdgenerate.cpp \
    private/generate/c++/icdgenerator_cpp.cpp \
    private/generate/c++/icdgenerator_cpp_p.cpp \
    private/generate/word/icdgenerator_word.cpp \
    private/generate/word/icdgenerator_word_p.cpp \
    private/generate/joffice.cpp \
    private/generate/word/jword.cpp \
    private/generate/html/icdgenerator_html.cpp \
    private/generate/html/icdgenerator_html_p.cpp \
    private/generate/pdf/icdgenerator_pdf.cpp \
    private/generate/pdf/icdgenerator_pdf_p.cpp \
    private/json/icdparser_json.cpp \
    private/xml/icdparser_xml.cpp \
    private/xml/icdparser_xml_save.cpp \
    private/xml/icdparser_xml_restore.cpp

##
include($$PWD/private/3rdpart/3rdpart.pri)

###############################################################
# resource files
###############################################################

RESOURCES += \
    resource/resource.qrc

###############################################################
# import files
###############################################################

###############################################################
# global commands
###############################################################

win32|unix: {
    commands += echo --- console - $$TARGET --- &

    excludefile = $$PWD/copy.ignore
    !exists("$$excludefile"):excludefile = "$$this_dir/source/common/copy.ignore"
    !exists("$$excludefile"):error("$$excludefile" is not exists!)
    win32:excludefile = $$replace(excludefile, /, \\)

    ## copy files
    dstdir = $$this_dir/include/core/icdparser
    win32:dstdir = $$replace(dstdir, /, \\)
    !exists("$$dstdir"):commands += $(MKDIR) "\"$$dstdir\"" &
    win32:srcdir = $$PWD/*.h
    unix:srcdir = $$PWD/
    win32:srcdir = $$replace(srcdir, /, \\)
    win32:commands += $(COPY_DIR) "\"$$srcdir\"" "\"$$dstdir\"" /exclude:"$$excludefile" &
    unix:commands += "\"$$this_dir/tools/xcopy.py\"" "\"$$srcdir\"" "\"$$dstdir\"" "*.h" &

    QMAKE_POST_LINK += $$commands
}
