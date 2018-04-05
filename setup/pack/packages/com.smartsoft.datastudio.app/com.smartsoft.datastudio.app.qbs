import qbs
import qbs.FileInfo
import qbs.Environment

PackProduct {

    Depends { name: 'cpp' }
    Depends { name: 'meta' }

    // global
    Group {
        name: 'data-global'
        prefix: project.sourceDirectory + '/'
        files: [
            //'CHANGE',
            'LICENSE',
            'README.md',
            'VERSION'
        ]
        qbs.install: true
        qbs.installPrefix: installPrefix
        qbs.installDir: installDir
    }

    // config
    Group {
        name: 'data-config'
        prefix: FileInfo.joinPaths(project.sourceDirectory, 'config') + '/'
        files: [ '**/*.*' ]
        excludeFiles: [ '**/*.pr[io]', '**/*.qbs' ]
        qbs.install: true
        qbs.installPrefix: installPrefix
        qbs.installDir: FileInfo.joinPaths(installDir, 'config')
        qbs.installSourceBase: prefix
    }

    // app
    Group {
        name: 'data-app'
        prefix: FileInfo.joinPaths(projectDir, 'bin') + '/'
        files: [ project.projectName + project.variantSuffix + '.exe' ]
        qbs.install: true
        qbs.installPrefix: installPrefix
        qbs.installDir: FileInfo.joinPaths(installDir, 'bin')
    }

    // tools
    Group {
        name: 'data-tools'
        prefix: FileInfo.joinPaths(project.sourceDirectory, 'tools') + '/'
        files: [ 'WinSnap/*', 'DataAnalyse.exe' ]
        qbs.install: true
        qbs.installPrefix: installPrefix
        qbs.installDir: FileInfo.joinPaths(installDir, 'tools')
        qbs.installSourceBase: prefix
    }

    // msvc-runtime
    Group {
        name: 'data-runtime'
        prefix: FileInfo.joinPaths(Environment.getEnv('VCINSTALLDIR'), 'redist',
                                   (qbs.buildVariant == 'debug' ? 'debug_*' : ''),
                                   qbs.architecture, 'Microsoft.VC*.*CRT') + '/'
        files: [ 'concrt*.dll', 'msvcp*.dll', 'vccorlib*.dll', 'vcruntime*.dll' ]
        qbs.install: true
        qbs.installPrefix: installPrefix
        qbs.installDir: FileInfo.joinPaths(installDir, 'bin')
    }
}
