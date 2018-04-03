import qbs
import '../3rdpart.qbs' as _3rdpart

_3rdpart {
    moduleName: 'jchart'

    Depends { name: '3rdpart.qwt' }
    Depends { name: '3rdpart.jwt' }
}
