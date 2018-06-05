import qbs
import '../3rdpart.qbs' as _3rdpart

_3rdpart {
    moduleName: 'qtftp'
    cpp.defines: [ 'QT_FTP_LIB' ]
}
