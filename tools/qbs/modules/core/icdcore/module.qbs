import qbs
import '../core.qbs' as _core

_core {
    moduleName: 'icdcore'
    cpp.defines: base.concat(['ICDCORE_JSON_LIB'])
}
