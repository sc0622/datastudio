import qbs
import '../3rdpart.qbs' as _3rdpart

_3rdpart {
    moduleName: 'rass'
    cpp.dynamicLibraries: [ 'rass/' + moduleName + variantSuffix + '_md' ]
}
