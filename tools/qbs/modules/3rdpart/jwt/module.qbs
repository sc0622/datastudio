import qbs
import '../3rdpart.qbs' as _3rdpart

_3rdpart {
    moduleName: 'jwt'

    Depends { name: 'Qt.widgets' }
}
