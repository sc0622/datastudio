import qbs

QtGuiApplication {

    readonly property path precompPath: project.sourceDirectory + '/src/common'

    cpp.includePaths: base.concat([
                                      precompPath,
                                      project.sourceDirectory + '/include',
                                      project.sourceDirectory + '/include/3rdpart',
                                      project.sourceDirectory + '/include/core'
                                  ])

    targetName: name
    cpp.variantSuffix: project.variantSuffix

    Depends { name: 'Qt.widgets' }

    Group {
        name: 'Precomp'
        condition: cpp.useCxxPrecompiledHeader
        prefix: precompPath + '/'
        files: ['precomp.h']
        fileTags: ['cpp_pch_src']
    }
}
