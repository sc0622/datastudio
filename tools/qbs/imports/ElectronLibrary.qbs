import qbs
import qbs.File
import qbs.FileInfo
import qbs.Environment

CppDyLibrary {
    type: base.concat([ 'node.out' ])
    //readonly property path nodePath: FileInfo.fromNativeSeparators(Environment.getEnv('NODE_PATH'))
    readonly property path userPath: '~'
    //readonly property path nodePath: '~/.node-gyp/8.11.1'
    readonly property path npmPath: '~/AppData/Roaming/npm'
    readonly property path npmModulesPath: FileInfo.joinPaths(npmPath, 'node_modules')
    readonly property path nodeAddonPath: FileInfo.joinPaths(npmModulesPath, 'node-addon-api')
    readonly property path iojsPath: FileInfo.joinPaths(userPath, '.electron-gyp/.node-gyp/iojs-2.0.0')
    property path destinationDir: destinationDirectory

    cpp.variantSuffix: ''
    cpp.includePaths: base.concat([nodeAddonPath,
                                   iojsPath + '/include/node',
                                   iojsPath + '/src',
                                   iojsPath + '/deps/uv/include',
                                   iojsPath + '/deps/v8/include'
                                  ])
    cpp.dynamicLibraries: base.concat([ FileInfo.joinPaths(userPath, '.electron-gyp/.node-gyp',
                                                           'iojs-2.0.0/x64/iojs'),
                                      'DelayImp'])
    cpp.defines: base.concat(['BUILDING_NODE_EXTENSION',
                              'NODE_GYP_MODULE_NAME=' + name,
                              'USING_UV_SHARED=1',
                              'USING_V8_SHARED=1',
                              //'V8_DEPRECATION_WARNINGS=1',
                              //'_HAS_EXCEPTIONS=0',
                              'V8_ENABLE_CHECKS'
                             ])
//    cpp.cxxFlags: ['/wd4267', '/wd4351', '/wd4355', '/wd4800', '/wd4251',
//        '/DELAYLOAD:"iojs.exe"', '/DELAYLOAD:"node.exe"' ]
//    cpp.cxxLanguageVersion: 'c++11'

//    Group {
//        name: 'load_hook'
//        prefix: FileInfo.joinPaths(npmModulesPath, 'node-gyp/src/')
//        files: [ 'win_delay_load_hook.cc' ]
//    }

    Rule {
        inputs: [ 'dynamiclibrary' ]
        Artifact {
            fileTags: [ 'node.out' ]
            filePath: FileInfo.joinPaths(product.destinationDir, input.completeBaseName + '.node')
        }
        prepare: {
            var cmd = new JavaScriptCommand;
            cmd.description = 'generating ' + output.fileName;
            cmd.sourceCode = function() { File.copy(input.filePath, output.filePath); }
            return [ cmd ];
        }
    }
}
