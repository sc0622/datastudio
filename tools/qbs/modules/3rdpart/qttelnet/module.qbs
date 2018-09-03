import qbs
import '../3rdpart.qbs' as _3rdpart

_3rdpart {
    moduleName: 'qttelnet'
    cpp.defines: [ 'QT_QTTELNET_LIB' ]
    cpp.dynamicLibraries: [ 'ws2_32' ]

    Depends { name: 'Qt.network' }
}
