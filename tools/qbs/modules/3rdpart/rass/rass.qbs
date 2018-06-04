import qbs
import '../3rdpart.qbs' as _3rdpart

_3rdpart {
    moduleName: 'rass'

    cpp.defines: [ ]
    cpp.libraryPaths: [ project.sourceDirectory + '/lib/3rdpart/' + moduleName ]
    cpp.dynamicLibraries: [ moduleName + (qbs.buildVariant == 'debug' ? 'd' : '') + '_md' ]
}
