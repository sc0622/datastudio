import qbs

Module {
    version: '1.0'

    property string moduleName: name
    property string variantSuffix: qbs.buildVariant == 'debug' ? 'd' : ''

    Depends { name: "cpp" }

    cpp.defines: [ moduleName.toUpperCase() + '_LIB' ]
    cpp.includePaths: [ project.sourceDirectory + '/include/3rdpart' ]
    cpp.libraryPaths: [ project.sourceDirectory + '/lib/3rdpart' ]
    cpp.dynamicLibraries: [ moduleName + variantSuffix ]
}
