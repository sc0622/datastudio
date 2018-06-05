import qbs
import '../3rdpart.qbs' as _3rdpart

_3rdpart {
    moduleName: 'qttelnet'
    cpp.defines: [ 'QT_QTTELNET_LIB' ]
}
