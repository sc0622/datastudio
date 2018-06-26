import qbs

Module {
    version: '1.0'

    property string moduleName: name
    property string variantSuffix: qbs.buildVariant == 'debug' ? 'd' : ''
    readonly property path includePaths: project.sourceDirectory + '/include/3rdpart'
    readonly property path libraryPaths: project.sourceDirectory + '/lib/3rdpart'

    Depends { name: "cpp" }

    cpp.defines: [ moduleName.toUpperCase() + '_LIB' ]
    cpp.includePaths: [ includePaths ]
    cpp.libraryPaths: [ libraryPaths ]
    cpp.dynamicLibraries: [ moduleName + variantSuffix ]
}
