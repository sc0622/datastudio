import qbs

Module {
    version: '1.0'

    property string moduleName: name

    Depends { name: "cpp" }

    cpp.defines: [ moduleName.toUpperCase() + '_LIB' ]
    cpp.includePaths: [ project.sourceDirectory + '/include/core' ]
    cpp.libraryPaths: [ project.sourceDirectory + '/lib/core' ]
    cpp.dynamicLibraries: [ moduleName + (qbs.buildVariant == 'debug' ? 'd' : '') ]
}
