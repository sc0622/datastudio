import qbs
import '../core.qbs' as _core

_core {
    moduleName: 'jfcsload'

    cpp.dynamicLibraries: [ 'ws2_32' ]

    Depends { name: 'Qt.network' }
    Depends { name: 'Qt.serialport' }
    Depends { name: '3rdpart.PCIS_DASK' }
}
