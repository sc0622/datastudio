import qbs
import '../3rdpart.qbs' as _3rdpart

_3rdpart {
    moduleName: 'log4cpp'

    Properties {
        condition: !qbs.targetOS.contains('windows')
        cpp.defines: base.concat(['LOG4CPP_FIX_ERROR_COLLISION'])
    }
}
