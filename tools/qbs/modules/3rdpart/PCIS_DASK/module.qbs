import qbs
import '../3rdpart.qbs' as _3rdpart

_3rdpart {
    moduleName: 'PCI-Dask'

    cpp.defines: [ 'PCIS_DASK_LIB' ]
    cpp.includePaths: [ project.sourceDirectory + '/lib/3rdpart/PCIS-DASK/Include' ]
    cpp.libraryPaths: [ project.sourceDirectory + '/lib/3rdpart/PCIS-DASK/Lib' ]
    cpp.dynamicLibraries: [ moduleName ]
}
