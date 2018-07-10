import qbs

Module {
    version: '1.0'

    property string moduleName: name
    property string variantSuffix: qbs.buildVariant == 'debug' ? 'd' : ''
    property path includePaths: project.sourceDirectory + '/include/core'
    property path libraryPaths: project.sourceDirectory + "/lib/core"

    Depends { name: "cpp" }

    cpp.defines: [ moduleName.toUpperCase() + '_LIB' ]
    cpp.includePaths: [ includePaths ]
    cpp.libraryPaths: [ libraryPaths ]
    cpp.dynamicLibraries: [ moduleName + variantSuffix ]
}
