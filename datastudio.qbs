import qbs
import 'tools/function.js' as Function

Project {

    qbs.enableDebugCode: true

    readonly property string dynamicSuffix: Function.dynamicSuffix(qbs)

    qbsSearchPaths: [
        'tools/qbs'
    ]

    /*
    Profile {
        name: 'globalInclude'
    }
*/
    qbs.profiles: [ 'globalInclude' ]

    references: [
        'src/src.qbs',
        'tools/tools.qbs'
    ]
}
