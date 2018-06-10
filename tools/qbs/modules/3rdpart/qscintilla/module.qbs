import '../3rdpart.qbs' as _3rdpart

_3rdpart {
    moduleName: 'qscintilla'
    cpp.includePaths: [
        project.sourceDirectory + '/include/3rdpart',
        project.sourceDirectory + '/include/3rdpart/' + moduleName
    ]
    cpp.defines: [ 'QSCINTILLA_DLL', 'SCINTILLA_QT', 'SCI_LEXER' ]
}
