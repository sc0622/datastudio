import qbs

Module {
    version: '1.0'

    property string moduleName: name

    Depends { name: "cpp" }

    cpp.defines: [ moduleName.toUpperCase() + '_LIB' ]
    cpp.includePaths: [ project.sourceDirectory + '/include/3rdpart' ]
    cpp.libraryPaths: [ project.sourceDirectory + '/lib/3rdpart' ]
    cpp.dynamicLibraries: [ moduleName + (qbs.buildVariant == 'debug' ? 'd' : '') ]
}
